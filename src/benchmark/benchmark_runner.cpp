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
#include "blacklist_manager.h"
#include "http_client.h"
#include "api_models.h"
#include <iostream>
#include <algorithm>
#include <cctype>

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

    // Use HttpClient for API request
    std::string result = HttpClient::get(apiUrl + "/models", apiKey);
    
    // Parse using existing ModelsListResponse
    ModelsListResponse response(result, provider);
    
    if (response.hasError()) {
        std::cerr << "Error: " << response.getErrorMessage() << std::endl;
        return models;
    }

    // Extract model IDs from response
    return extractModelIds(response.getModels());
}

std::vector<std::string> BenchmarkRunner::extractModelIds(const std::vector<ModelInfo>& models) {
    std::vector<std::string> modelIds;
    modelIds.reserve(models.size());
    
    for (const auto& model : models) {
        modelIds.push_back(model.id);
    }
    
    return modelIds;
}

Json::Value BenchmarkRunner::buildBenchmarkMessages(const std::string& testPrompt) {
    Json::Value messages(Json::arrayValue);
    Json::Value message;
    message["role"] = "user";
    message["content"] = testPrompt;
    messages.append(message);
    return messages;
}

bool BenchmarkRunner::shouldBlacklistModel(const std::string& errorMessage) {
    // Check for common error patterns that indicate incompatible models
    const std::vector<std::string> blacklistPatterns = {
        "does not support chat completions",
        "model not found",
        "model does not exist",
        "invalid model",
        "unsupported model",
        "deprecated model"
    };
    
    std::string lowerError = errorMessage;
    std::transform(lowerError.begin(), lowerError.end(), lowerError.begin(), ::tolower);
    
    for (const std::string& pattern : blacklistPatterns) {
        if (lowerError.find(pattern) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> BenchmarkRunner::filterBlacklistedModels(
    const std::vector<std::string>& allModels, 
    const std::string& provider,
    size_t& blacklistedCount) {
    
    std::vector<std::string> modelsToTest;
    blacklistedCount = 0;
    
    for (const std::string& model : allModels) {
        if (BlacklistManager::isModelBlacklisted(provider, model)) {
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

    // Build chat messages for the request
    Json::Value messages = buildBenchmarkMessages(testPrompt);

    // Create chat request using existing API models
    ChatRequest chatRequest(model, messages);

    // Display progress
    BenchmarkReporter::displayTestStart(model);

    // Execute timed API request using HttpClient
    PerformanceTimer timer;
    timer.start();

    std::string response = HttpClient::post(apiUrl + "/chat/completions", apiKey, chatRequest.toJson());

    result.responseTimeMs = timer.stop();

    // Process response using ChatResponse
    ChatResponse chatResponse(response);
    
    if (chatResponse.hasError()) {
        result.errorMessage = chatResponse.getErrorMessage();
        
        // Check if this error indicates the model should be blacklisted
        if (shouldBlacklistModel(chatResponse.getErrorMessage())) {
            BlacklistManager::addModelToBlacklist(provider, model, chatResponse.getErrorMessage());
        }
    } else {
        result.success = true;
    }

    // Display result
    BenchmarkReporter::displayTestResult(result);

    return result;
}
