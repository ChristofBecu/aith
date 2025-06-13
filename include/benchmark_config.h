#ifndef BENCHMARK_CONFIG_H
#define BENCHMARK_CONFIG_H

#include <string>

/**
 * Configuration management for benchmark operations
 * Centralizes all benchmark-related configuration settings
 */
class BenchmarkConfig {
public:
    // Default configuration values
    static const std::string DEFAULT_TEST_PROMPT;
    static const int DEFAULT_MAX_TOKENS;
    static const std::string TEMP_FILE_PREFIX;
    static const std::string TEMP_HISTORY_PREFIX;
    
    /**
     * Get the default test prompt for benchmarks
     * @return Default prompt string
     */
    static std::string getDefaultTestPrompt();
    
    /**
     * Get the maximum tokens for benchmark requests
     * @return Maximum tokens limit
     */
    static int getMaxTokens();
    
    /**
     * Get the temporary file prefix for benchmark payloads
     * @return Temporary file prefix
     */
    static std::string getTempFilePrefix();
    
    /**
     * Get the temporary history file prefix
     * @return Temporary history file prefix
     */
    static std::string getTempHistoryPrefix();
    
    /**
     * Generate a temporary file path for benchmark payload
     * @param modelName Sanitized model name
     * @return Full temporary file path
     */
    static std::string getTempPayloadPath(const std::string& modelName);
    
    /**
     * Generate a temporary history file path
     * @param modelName Sanitized model name
     * @return Full temporary history file path
     */
    static std::string getTempHistoryPath(const std::string& modelName);
    
    /**
     * Check if results should be sorted by response time
     * @return True if sorting by response time is enabled
     */
    static bool shouldSortByResponseTime();
    
    /**
     * Check if successful tests should be prioritized in sorting
     * @return True if successful tests should be shown first
     */
    static bool shouldPrioritizeSuccessfulTests();
    
    /**
     * Get the precision for displaying response times
     * @return Number of decimal places for response time display
     */
    static int getResponseTimePrecision();
};

#endif // BENCHMARK_CONFIG_H
