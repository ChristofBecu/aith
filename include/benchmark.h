#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <vector>
#include <chrono>
#include "benchmark_config.h"

/**
 * Structure to store benchmark result for a single model
 */
struct BenchmarkResult {
    std::string provider;
    std::string model;
    double responseTimeMs;
    bool success;
    std::string errorMessage;
    std::string timestamp;
};

/**
 * Runs a benchmark test on a single model
 * @param provider The provider name
 * @param model The model name
 * @param apiKey The API key for authentication
 * @param testPrompt The prompt to use for testing
 * @return BenchmarkResult with timing and success information
 */
BenchmarkResult runModelBenchmark(const std::string &provider, const std::string &model, 
                                  const std::string &apiKey, const std::string &testPrompt = "");

/**
 * Runs benchmark tests on all available models for the current provider
 * @param apiKey The API key for authentication
 * @param testPrompt The prompt to use for testing
 * @return Vector of BenchmarkResult sorted by response time
 */
std::vector<BenchmarkResult> runAllModelsBenchmark(const std::string &apiKey, const std::string &testPrompt = "");

/**
 * Displays benchmark results in a formatted table
 * @param results Vector of BenchmarkResult to display
 */
void displayBenchmarkResults(const std::vector<BenchmarkResult> &results);

/**
 * Gets all available models for the current provider
 * @param apiKey The API key for authentication
 * @return Vector of model names
 */
std::vector<std::string> getAvailableModels(const std::string &apiKey);

#endif // BENCHMARK_H
