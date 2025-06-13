#include "benchmark.h"
#include "benchmark_runner.h"
#include "benchmark_reporter.h"

/**
 * Runs a benchmark test on a single model
 */
BenchmarkResult runModelBenchmark(const std::string &provider, const std::string &model, 
                                  const std::string &apiKey, const std::string &testPrompt) {
    return BenchmarkRunner::runSingleModel(provider, model, apiKey, testPrompt);
}

/**
 * Gets all available models for the current provider
 */
std::vector<std::string> getAvailableModels(const std::string &apiKey) {
    return BenchmarkRunner::getAvailableModels(apiKey);
}

/**
 * Runs benchmark tests on all available models for the current provider
 */
std::vector<BenchmarkResult> runAllModelsBenchmark(const std::string &apiKey, const std::string &testPrompt) {
    return BenchmarkRunner::runAllModels(apiKey, testPrompt);
}

/**
 * Displays benchmark results in a formatted table
 */
void displayBenchmarkResults(const std::vector<BenchmarkResult> &results) {
    BenchmarkReporter::displayResults(results);
}
