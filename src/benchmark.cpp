#include "benchmark.h"
#include "benchmark_config.h"
#include "benchmark_request.h"
#include "benchmark_response.h"
#include "performance_timer.h"
#include "benchmark_reporter.h"
#include "system_utils.h"
#include "string_utils.h"
#include "file_utils.h"
#include "provider_manager.h"
#include "model_blacklist.h"
#include "api.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

/**
 * Runs a benchmark test on a single model
 */
BenchmarkResult runModelBenchmark(const std::string &provider, const std::string &model, 
                                  const std::string &apiKey, const std::string &testPrompt) {
    BenchmarkResult result;
    result.provider = provider;
    result.model = model;
    result.timestamp = SystemUtils::getCurrentTimestamp();
    result.success = false;
    result.responseTimeMs = 0.0;

    // Use default test prompt if none provided
    std::string actualTestPrompt = testPrompt.empty() ? BenchmarkConfig::getDefaultTestPrompt() : testPrompt;

    std::string apiUrl = ProviderManager::getApiUrl();
    if (apiUrl.empty()) {
        result.errorMessage = "No API URL configured for provider";
        return result;
    }

    // Create a temporary history file for this test
    std::string sanitizedModelName = StringUtils::sanitizeForFilename(model);
    std::string tempHistory = FileUtils::createTempJsonFile("[]", "benchmark_history_" + sanitizedModelName);

    // Create benchmark request
    BenchmarkRequest request(model, actualTestPrompt, BenchmarkConfig::getMaxTokens());
    
    // Validate request
    if (!request.isValid()) {
        result.errorMessage = "Invalid request: " + request.getValidationError();
        FileUtils::removeFile(tempHistory);
        return result;
    }
    
    // Create temporary file with request payload
    std::string tempFile = request.createTempFile(sanitizedModelName);

    BenchmarkReporter::displayTestStart(model);

    // Create performance timer and start measuring
    PerformanceTimer timer;
    timer.start();

    // Send request to API
    std::string command = "curl -s -X POST " + apiUrl + "/chat/completions -H 'Authorization: Bearer " + apiKey + "' -H 'Content-Type: application/json' -d @" + tempFile;
    std::string response = SystemUtils::exec(command.c_str());

    // Stop timer and record duration
    result.responseTimeMs = timer.stop();

    // Parse and analyze response
    BenchmarkResponse benchmarkResponse(response);
    
    if (benchmarkResponse.isSuccessful()) {
        result.success = true;
    } else {
        result.errorMessage = benchmarkResponse.getErrorMessage();
        
        // Auto-blacklist models that don't support chat completions
        if (benchmarkResponse.shouldBlacklist()) {
            ModelBlacklist::addModelToBlacklist(provider, model, benchmarkResponse.getBlacklistReason());
        }
    }

    // Display the test result
    BenchmarkReporter::displayTestResult(result);

    // Clean up temporary files
    FileUtils::removeFiles({tempFile, tempHistory});

    return result;
}

/**
 * Gets all available models for the current provider
 */
std::vector<std::string> getAvailableModels(const std::string &apiKey) {
    std::vector<std::string> models;
    std::string apiUrl = ProviderManager::getApiUrl();
    std::string provider = ProviderManager::getAgent();
    
    if (apiUrl.empty()) {
        std::cerr << "Error: No API URL configured for provider '" << provider << "'" << std::endl;
        return models;
    }

    std::string command = "curl -s -X GET " + apiUrl + "/models -H 'Authorization: Bearer " + apiKey + "'";
    std::string result = SystemUtils::exec(command.c_str());

    Json::Value data;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(result);
    if (!Json::parseFromStream(reader, s, &data, &errs)) {
        std::cerr << "Error parsing models JSON: " << errs << std::endl;
        return models;
    }

    // Parse models from response (similar to listModels function)
    if (data.isMember("data") && data["data"].isArray()) {
        for (const auto &model : data["data"]) {
            if (model.isMember("id")) {
                std::string modelId = model["id"].asString();
                models.push_back(modelId);
            }
        }
    } else if (data.isMember("models") && data["models"].isArray()) {
        for (const auto &model : data["models"]) {
            if (model.isMember("id")) {
                std::string modelId = model["id"].asString();
                models.push_back(modelId);
            }
        }
    }

    return models;
}

/**
 * Runs benchmark tests on all available models for the current provider
 */
std::vector<BenchmarkResult> runAllModelsBenchmark(const std::string &apiKey, const std::string &testPrompt) {
    std::vector<BenchmarkResult> results;
    std::string provider = ProviderManager::getAgent();

    // Use default test prompt if none provided
    std::string actualTestPrompt = testPrompt.empty() ? BenchmarkConfig::getDefaultTestPrompt() : testPrompt;

    BenchmarkReporter::displayBenchmarkHeader(provider, actualTestPrompt);

    // Get all available models
    std::vector<std::string> allModels = getAvailableModels(apiKey);
    
    if (allModels.empty()) {
        BenchmarkReporter::displayNoModelsError(provider);
        return results;
    }

    // Filter out blacklisted models
    std::vector<std::string> modelsToTest;
    size_t blacklistedCount = 0;
    
    for (const std::string &model : allModels) {
        if (ModelBlacklist::isModelBlacklisted(provider, model)) {
            blacklistedCount++;
        } else {
            modelsToTest.push_back(model);
        }
    }

    // Display filtering information
    BenchmarkReporter::displayModelCounts(allModels.size(), blacklistedCount, modelsToTest.size());

    // Test each non-blacklisted model
    for (const std::string &model : modelsToTest) {
        BenchmarkResult result = runModelBenchmark(provider, model, apiKey, actualTestPrompt);
        results.push_back(result);
    }

    // Sort results by response time (successful tests first, then by time)
    if (BenchmarkConfig::shouldSortByResponseTime()) {
        std::sort(results.begin(), results.end(), [](const BenchmarkResult &a, const BenchmarkResult &b) {
            if (BenchmarkConfig::shouldPrioritizeSuccessfulTests() && a.success != b.success) {
                return a.success; // Successful tests first
            }
            if (a.success && b.success) {
                return a.responseTimeMs < b.responseTimeMs; // Faster times first
            }
            return false; // Failed tests maintain their order
        });
    }

    return results;
}

/**
 * Displays benchmark results in a formatted table
 */
void displayBenchmarkResults(const std::vector<BenchmarkResult> &results) {
    BenchmarkReporter::displayResults(results);
}
