#pragma once

#include <string>
#include <vector>

// Forward declaration to avoid circular dependency
struct BenchmarkResult;

/**
 * Handles all display and formatting for benchmark operations
 * 
 * This class centralizes all console output formatting for benchmark results,
 * progress indicators, and summary reports. It provides a clean separation
 * between business logic and presentation logic.
 * 
 * Usage:
 *   BenchmarkReporter::displayTestStart("model-name");
 *   BenchmarkReporter::displayTestResult(result);
 *   BenchmarkReporter::displayResults(results);
 */
class BenchmarkReporter {
public:
    // Progress reporting during individual tests
    /**
     * Display the start of a model test
     * @param model The model name being tested
     */
    static void displayTestStart(const std::string& model);

    /**
     * Display the result of a single model test
     * @param result The benchmark result to display
     */
    static void displayTestResult(const BenchmarkResult& result);

    // Batch operation reporting
    /**
     * Display header information for a benchmark run
     * @param provider The provider name
     * @param testPrompt The test prompt being used
     */
    static void displayBenchmarkHeader(const std::string& provider, const std::string& testPrompt);

    /**
     * Display model count information
     * @param totalModels Total number of models found
     * @param blacklistedCount Number of blacklisted models
     * @param testingCount Number of models to be tested
     */
    static void displayModelCounts(size_t totalModels, size_t blacklistedCount, size_t testingCount);

    /**
     * Display error when no models are found
     * @param provider The provider name
     */
    static void displayNoModelsError(const std::string& provider);

    // Complete results display
    /**
     * Display complete benchmark results with summary
     * @param results Vector of benchmark results to display
     */
    static void displayResults(const std::vector<BenchmarkResult>& results);

private:
    // Internal formatting methods
    /**
     * Display successful test results
     * @param successful Vector of successful results
     */
    static void displaySuccessfulResults(const std::vector<BenchmarkResult>& successful);

    /**
     * Display failed test results
     * @param failed Vector of failed results
     */
    static void displayFailedResults(const std::vector<BenchmarkResult>& failed);

    /**
     * Display summary statistics
     * @param results All benchmark results
     */
    static void displaySummary(const std::vector<BenchmarkResult>& results);

    // Formatting helpers
    /**
     * Format response time with appropriate precision
     * @param timeMs Response time in milliseconds
     * @return Formatted time string
     */
    static std::string formatResponseTime(double timeMs);

    /**
     * Format a model entry for results display
     * @param result The benchmark result
     * @param index Optional index number for numbered lists
     * @return Formatted model entry string
     */
    static std::string formatModelEntry(const BenchmarkResult& result, size_t index = 0);

    /**
     * Print a visual separator line
     */
    static void printSeparator();
};
