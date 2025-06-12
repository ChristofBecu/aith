#include "benchmark.h"
#include "system_utils.h"
#include "provider_manager.h"
#include "model_blacklist.h"
#include "api.h"
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <iomanip>

/**
 * Gets the current timestamp as a string
 */
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

/**
 * Sanitizes a model name for use in file paths
 * Replaces problematic characters with safe alternatives
 */
std::string sanitizeModelName(const std::string &modelName) {
    std::string sanitized = modelName;
    
    // Replace problematic characters
    for (char &c : sanitized) {
        switch (c) {
            case '/':  c = '_'; break;  // Directory separator
            case ':':  c = '-'; break;  // Can cause issues on some systems
            case ' ':  c = '_'; break;  // Spaces in filenames
            case '\\': c = '_'; break;  // Windows directory separator
            case '?':  c = '_'; break;  // Query character
            case '*':  c = '_'; break;  // Wildcard
            case '<':  c = '_'; break;  // Redirection
            case '>':  c = '_'; break;  // Redirection
            case '|':  c = '_'; break;  // Pipe
            case '"':  c = '_'; break;  // Quote
            // Keep other characters as-is (alphanumeric, -, _, .)
        }
    }
    
    return sanitized;
}

/**
 * Runs a benchmark test on a single model
 */
BenchmarkResult runModelBenchmark(const std::string &provider, const std::string &model, 
                                  const std::string &apiKey, const std::string &testPrompt) {
    BenchmarkResult result;
    result.provider = provider;
    result.model = model;
    result.timestamp = getCurrentTimestamp();
    result.success = false;
    result.responseTimeMs = 0.0;

    std::string apiUrl = ProviderManager::getApiUrl();
    if (apiUrl.empty()) {
        result.errorMessage = "No API URL configured for provider";
        return result;
    }

    // Create a temporary history file for this test
    std::string sanitizedModelName = sanitizeModelName(model);
    std::string tempHistory = "/tmp/benchmark_history_" + sanitizedModelName + ".json";
    std::ofstream historyFile(tempHistory);
    historyFile << "[]";
    historyFile.close();

    // Build the request payload
    Json::Value payload;
    payload["model"] = model;
    
    Json::Value messages(Json::arrayValue);
    Json::Value userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = testPrompt;
    messages.append(userMessage);
    payload["messages"] = messages;

    // Set a reasonable max_tokens to avoid very long responses
    payload["max_tokens"] = 50;

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    std::string payloadJson = Json::writeString(writer, payload);

    // Save payload to temp file
    std::string tempFile = "/tmp/benchmark_payload_" + sanitizedModelName + ".json";
    std::ofstream outFile(tempFile);
    outFile << payloadJson;
    outFile.close();

    std::cout << "Testing " << model << "... " << std::flush;

    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();

    // Send request to API
    std::string command = "curl -s -X POST " + apiUrl + "/chat/completions -H 'Authorization: Bearer " + apiKey + "' -H 'Content-Type: application/json' -d @" + tempFile;
    std::string response = SystemUtils::exec(command.c_str());

    // Record end time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    result.responseTimeMs = duration.count();

    // Parse response to check if it was successful
    Json::Value data;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(response);
    
    if (!Json::parseFromStream(reader, s, &data, &errs)) {
        result.errorMessage = "Failed to parse JSON response";
        std::cout << "✗ (JSON parse error)" << std::endl;
    } else if (!data.isMember("choices") || !data["choices"][0].isMember("message")) {
        result.errorMessage = "Invalid API response format";
        // Auto-blacklist models that don't support chat completions
        ModelBlacklist::addModelToBlacklist(provider, model, "Auto-blacklisted: Invalid API response format");
        std::cout << "✗ (Invalid response - auto-blacklisted)" << std::endl;
    } else if (data.isMember("error")) {
        result.errorMessage = data["error"]["message"].asString();
        std::cout << "✗ (" << result.errorMessage << ")" << std::endl;
    } else {
        result.success = true;
        std::cout << "✓ (" << result.responseTimeMs << "ms)" << std::endl;
    }

    // Clean up temporary files
    std::filesystem::remove(tempFile);
    std::filesystem::remove(tempHistory);

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

    std::cout << "Running benchmark tests for provider '" << provider << "'..." << std::endl;
    std::cout << "Test prompt: \"" << testPrompt << "\"" << std::endl;
    std::cout << std::endl;

    // Get all available models
    std::vector<std::string> allModels = getAvailableModels(apiKey);
    
    if (allModels.empty()) {
        std::cerr << "No models found for provider '" << provider << "'" << std::endl;
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
    std::cout << "Found " << allModels.size() << " models total";
    if (blacklistedCount > 0) {
        std::cout << ", filtering out " << blacklistedCount << " blacklisted models";
    }
    std::cout << std::endl;
    std::cout << "Testing " << modelsToTest.size() << " models:" << std::endl;

    // Test each non-blacklisted model
    for (const std::string &model : modelsToTest) {
        BenchmarkResult result = runModelBenchmark(provider, model, apiKey, testPrompt);
        results.push_back(result);
    }

    // Sort results by response time (successful tests first, then by time)
    std::sort(results.begin(), results.end(), [](const BenchmarkResult &a, const BenchmarkResult &b) {
        if (a.success != b.success) {
            return a.success; // Successful tests first
        }
        if (a.success && b.success) {
            return a.responseTimeMs < b.responseTimeMs; // Faster times first
        }
        return false; // Failed tests maintain their order
    });

    return results;
}

/**
 * Displays benchmark results in a formatted table
 */
void displayBenchmarkResults(const std::vector<BenchmarkResult> &results) {
    if (results.empty()) {
        std::cout << "No benchmark results to display." << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << "=== BENCHMARK RESULTS ===" << std::endl;
    std::cout << std::endl;

    // Display successful results
    std::vector<BenchmarkResult> successful;
    std::vector<BenchmarkResult> failed;
    
    for (const auto &result : results) {
        if (result.success) {
            successful.push_back(result);
        } else {
            failed.push_back(result);
        }
    }

    if (!successful.empty()) {
        std::cout << "✓ Successful tests (sorted by speed):" << std::endl;
        for (size_t i = 0; i < successful.size(); ++i) {
            const auto &result = successful[i];
            std::cout << (i + 1) << ". " << result.model << " (" << result.provider << ") - " 
                      << std::fixed << std::setprecision(0) << result.responseTimeMs << "ms" << std::endl;
        }
        std::cout << std::endl;
    }

    if (!failed.empty()) {
        std::cout << "✗ Failed tests:" << std::endl;
        for (const auto &result : failed) {
            std::cout << result.model << " (" << result.provider << ") - " << result.errorMessage << std::endl;
        }
        std::cout << std::endl;
    }

    // Summary
    std::cout << "Summary: " << successful.size() << " successful, " << failed.size() << " failed" << std::endl;
    
    if (!successful.empty()) {
        const auto &fastest = successful[0];
        std::cout << "Fastest model: " << fastest.model << " (" << fastest.responseTimeMs << "ms)" << std::endl;
    }
}
