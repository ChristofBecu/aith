#pragma once

#include <string>
#include <vector>
#include <json/json.h>

// Forward declarations to avoid circular dependencies
struct BenchmarkResult;
struct ModelInfo;

/**
 * Orchestrates benchmark execution workflows
 * 
 * This class centralizes all benchmark execution orchestration, coordinating
 * between configuration, requests, responses, timing, and reporting components.
 * It provides clean interfaces for single model and batch benchmark operations.
 * 
 * Usage:
 *   BenchmarkResult result = BenchmarkRunner::runSingleModel(provider, model, apiKey);
 *   std::vector<BenchmarkResult> results = BenchmarkRunner::runAllModels(apiKey);
 */
class BenchmarkRunner {
public:
    // Core execution methods
    /**
     * Execute a benchmark test on a single model
     * @param provider The provider name
     * @param model The model name
     * @param apiKey The API key for authentication
     * @param testPrompt The prompt to use for testing (empty = use default)
     * @return BenchmarkResult with timing and success information
     */
    static BenchmarkResult runSingleModel(const std::string& provider, 
                                         const std::string& model, 
                                         const std::string& apiKey,
                                         const std::string& testPrompt = "");

    /**
     * Execute benchmark tests on all available models for the current provider
     * @param apiKey The API key for authentication
     * @param testPrompt The prompt to use for testing (empty = use default)
     * @return Vector of benchmark results for all tested models
     */
    static std::vector<BenchmarkResult> runAllModels(const std::string& apiKey,
                                                    const std::string& testPrompt = "");

    // Model discovery
    /**
     * Get all available models for the current provider
     * @param apiKey The API key for authentication
     * @return Vector of available model names
     */
    static std::vector<std::string> getAvailableModels(const std::string& apiKey);

private:
    // Helper orchestration methods
    /**
     * Filter out blacklisted models from a list
     * @param allModels Vector of all available models
     * @param provider The provider name for blacklist lookup
     * @param blacklistedCount Output parameter for number of filtered models
     * @return Vector of non-blacklisted models
     */
    static std::vector<std::string> filterBlacklistedModels(
        const std::vector<std::string>& allModels, 
        const std::string& provider,
        size_t& blacklistedCount);

    /**
     * Sort benchmark results according to configuration settings
     * @param results Vector of results to sort in-place
     */
    static void sortResults(std::vector<BenchmarkResult>& results);

    /**
     * Create a benchmark result with error information
     * @param provider The provider name
     * @param model The model name
     * @param errorMessage The error message
     * @return BenchmarkResult with error details
     */
    static BenchmarkResult createErrorResult(const std::string& provider, 
                                            const std::string& model, 
                                            const std::string& errorMessage);

    /**
     * Execute the core API request workflow for a single model
     * @param provider The provider name
     * @param model The model name
     * @param apiKey The API key
     * @param testPrompt The test prompt
     * @param apiUrl The API URL
     * @return BenchmarkResult with execution details
     */
    static BenchmarkResult executeModelRequest(const std::string& provider,
                                              const std::string& model,
                                              const std::string& apiKey,
                                              const std::string& testPrompt,
                                              const std::string& apiUrl);

    /**
     * Extract model IDs from ModelsListResponse for benchmarking
     * @param response The models list response from API
     * @return Vector of model ID strings
     */
    static std::vector<std::string> extractModelIds(const std::vector<ModelInfo>& models);

    /**
     * Build simple benchmark messages for chat completion request
     * @param testPrompt The test prompt to send
     * @return JSON messages array for chat completion
     */
    static Json::Value buildBenchmarkMessages(const std::string& testPrompt);

    /**
     * Determine if a model should be blacklisted based on error message
     * @param errorMessage The error message from API response
     * @return True if the model should be blacklisted
     */
    static bool shouldBlacklistModel(const std::string& errorMessage);
};
