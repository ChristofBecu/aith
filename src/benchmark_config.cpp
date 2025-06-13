#include "benchmark_config.h"

// Default configuration constants
const std::string BenchmarkConfig::DEFAULT_TEST_PROMPT = "1+1";
const int BenchmarkConfig::DEFAULT_MAX_TOKENS = 50;
const std::string BenchmarkConfig::TEMP_FILE_PREFIX = "/tmp/benchmark_payload_";
const std::string BenchmarkConfig::TEMP_HISTORY_PREFIX = "/tmp/benchmark_history_";

std::string BenchmarkConfig::getDefaultTestPrompt() {
    return DEFAULT_TEST_PROMPT;
}

int BenchmarkConfig::getMaxTokens() {
    return DEFAULT_MAX_TOKENS;
}

std::string BenchmarkConfig::getTempFilePrefix() {
    return TEMP_FILE_PREFIX;
}

std::string BenchmarkConfig::getTempHistoryPrefix() {
    return TEMP_HISTORY_PREFIX;
}

std::string BenchmarkConfig::getTempPayloadPath(const std::string& modelName) {
    return TEMP_FILE_PREFIX + modelName + ".json";
}

std::string BenchmarkConfig::getTempHistoryPath(const std::string& modelName) {
    return TEMP_HISTORY_PREFIX + modelName + ".json";
}

bool BenchmarkConfig::shouldSortByResponseTime() {
    return true; // Default behavior: sort by response time
}

bool BenchmarkConfig::shouldPrioritizeSuccessfulTests() {
    return true; // Default behavior: show successful tests first
}

int BenchmarkConfig::getResponseTimePrecision() {
    return 0; // Default behavior: no decimal places for response time
}
