#include "benchmark_runner.h"
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
#include <sstream>
#include <json/json.h>

BenchmarkResult BenchmarkRunner::runSingleModel(const std::string& provider, 
                                               const std::string& model, 
                                               const std::string& apiKey,
                                               const std::string& testPrompt) {
    // Initialize result structure
    BenchmarkResult result;
    result.provider = provider;
    result.model = model;
    result.timestamp = SystemUtils::getCurrentTimestamp();
    result.success = false;
    result.responseTimeMs = 0.0;

    // Use default test prompt if none provided
    std::string actualTestPrompt = testPrompt.empty() ? BenchmarkConfig::getDefaultTestPrompt() : testPrompt;

    // Validate API configuration
    std::string apiUrl = ProviderManager::getApiUrl();
    if (apiUrl.empty()) {
        return createErrorResult(provider, model, "No API URL configured for provider");
    }

    // Execute the request workflow
    return executeModelRequest(provider, model, apiKey, actualTestPrompt, apiUrl);
}

std::vector<BenchmarkResult> BenchmarkRunner::runAllModels(const std::string& apiKey,
                                                          const std::string& testPrompt) {
    std::vector<BenchmarkResult> results;
    std::string provider = ProviderManager::getAgent();

    // Use default test prompt if none provided
    std::string actualTestPrompt = testPrompt.empty() ? BenchmarkConfig::getDefaultTestPrompt() : testPrompt;

    // Display benchmark header
    BenchmarkReporter::displayBenchmarkHeader(provider, actualTestPrompt);

    // Get all available models
    std::vector<std::string> allModels = getAvailableModels(apiKey);
    
    if (allModels.empty()) {
        BenchmarkReporter::displayNoModelsError(provider);
        return results;
    }

    // Filter out blacklisted models
    size_t blacklistedCount = 0;
    std::vector<std::string> modelsToTest = filterBlacklistedModels(allModels, provider, blacklistedCount);

    // Display filtering information
    BenchmarkReporter::displayModelCounts(allModels.size(), blacklistedCount, modelsToTest.size());

    // Test each non-blacklisted model
    for (const std::string& model : modelsToTest) {
        BenchmarkResult result = runSingleModel(provider, model, apiKey, actualTestPrompt);
        results.push_back(result);
    }

    // Sort results according to configuration
    sortResults(results);

    return results;
}

std::vector<std::string> BenchmarkRunner::getAvailableModels(const std::string& apiKey) {
    std::vector<std::string> models;
    std::string apiUrl = ProviderManager::getApiUrl();
    std::string provider = ProviderManager::getAgent();
    
    if (apiUrl.empty()) {
        std::cerr << "Error: No API URL configured for provider '" << provider << "'" << std::endl;
        return models;
    }

    // Execute models API request
    std::string command = "curl -s -X GET " + apiUrl + "/models -H 'Authorization: Bearer " + apiKey + "'";
    std::string result = SystemUtils::exec(command.c_str());

    // Parse JSON response
    Json::Value data;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(result);
    if (!Json::parseFromStream(reader, s, &data, &errs)) {
        std::cerr << "Error parsing models JSON: " << errs << std::endl;
        return models;
    }

    // Extract model IDs from response
    if (data.isMember("data") && data["data"].isArray()) {
        for (const auto& model : data["data"]) {
            if (model.isMember("id")) {
                std::string modelId = model["id"].asString();
                models.push_back(modelId);
            }
        }
    } else if (data.isMember("models") && data["models"].isArray()) {
        for (const auto& model : data["models"]) {
            if (model.isMember("id")) {
                std::string modelId = model["id"].asString();
                models.push_back(modelId);
            }
        }
    }

    return models;
}

std::vector<std::string> BenchmarkRunner::filterBlacklistedModels(
    const std::vector<std::string>& allModels, 
    const std::string& provider,
    size_t& blacklistedCount) {
    
    std::vector<std::string> modelsToTest;
    blacklistedCount = 0;
    
    for (const std::string& model : allModels) {
        if (ModelBlacklist::isModelBlacklisted(provider, model)) {
            blacklistedCount++;
        } else {
            modelsToTest.push_back(model);
        }
    }
    
    return modelsToTest;
}

void BenchmarkRunner::sortResults(std::vector<BenchmarkResult>& results) {
    if (BenchmarkConfig::shouldSortByResponseTime()) {
        std::sort(results.begin(), results.end(), [](const BenchmarkResult& a, const BenchmarkResult& b) {
            if (BenchmarkConfig::shouldPrioritizeSuccessfulTests() && a.success != b.success) {
                return a.success; // Successful tests first
            }
            if (a.success && b.success) {
                return a.responseTimeMs < b.responseTimeMs; // Faster times first
            }
            return false; // Failed tests maintain their order
        });
    }
}

BenchmarkResult BenchmarkRunner::createErrorResult(const std::string& provider, 
                                                  const std::string& model, 
                                                  const std::string& errorMessage) {
    BenchmarkResult result;
    result.provider = provider;
    result.model = model;
    result.timestamp = SystemUtils::getCurrentTimestamp();
    result.success = false;
    result.responseTimeMs = 0.0;
    result.errorMessage = errorMessage;
    return result;
}

BenchmarkResult BenchmarkRunner::executeModelRequest(const std::string& provider,
                                                    const std::string& model,
                                                    const std::string& apiKey,
                                                    const std::string& testPrompt,
                                                    const std::string& apiUrl) {
    BenchmarkResult result;
    result.provider = provider;
    result.model = model;
    result.timestamp = SystemUtils::getCurrentTimestamp();
    result.success = false;
    result.responseTimeMs = 0.0;

    // Create temporary history file for this test
    std::string sanitizedModelName = StringUtils::sanitizeForFilename(model);
    std::string tempHistory = FileUtils::createTempJsonFile("[]", "benchmark_history_" + sanitizedModelName);

    // Create and validate benchmark request
    BenchmarkRequest request(model, testPrompt, BenchmarkConfig::getMaxTokens());
    
    if (!request.isValid()) {
        result.errorMessage = "Invalid request: " + request.getValidationError();
        FileUtils::removeFile(tempHistory);
        return result;
    }
    
    // Create temporary file with request payload
    std::string tempFile = request.createTempFile(sanitizedModelName);

    // Display progress
    BenchmarkReporter::displayTestStart(model);

    // Execute timed API request
    PerformanceTimer timer;
    timer.start();

    std::string command = "curl -s -X POST " + apiUrl + "/chat/completions -H 'Authorization: Bearer " + apiKey + "' -H 'Content-Type: application/json' -d @" + tempFile;
    std::string response = SystemUtils::exec(command.c_str());

    result.responseTimeMs = timer.stop();

    // Process response
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

    // Display result
    BenchmarkReporter::displayTestResult(result);

    // Clean up temporary files
    FileUtils::removeFiles({tempFile, tempHistory});

    return result;
}
