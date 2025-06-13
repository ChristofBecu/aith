#include "benchmark_reporter.h"
#include "benchmark.h"
#include "benchmark_config.h"
#include <iostream>
#include <iomanip>

void BenchmarkReporter::displayTestStart(const std::string& model) {
    std::cout << "Testing " << model << "... " << std::flush;
}

void BenchmarkReporter::displayTestResult(const BenchmarkResult& result) {
    if (result.success) {
        std::cout << "✓ (" << formatResponseTime(result.responseTimeMs) << ")" << std::endl;
    } else {
        // Determine the appropriate error message format
        if (result.errorMessage.find("auto-blacklisted") != std::string::npos ||
            result.errorMessage.find("Invalid response") != std::string::npos) {
            std::cout << "✗ (Invalid response - auto-blacklisted)" << std::endl;
        } else if (result.errorMessage.find("JSON parse error") != std::string::npos ||
                   result.errorMessage.find("parse") != std::string::npos) {
            std::cout << "✗ (JSON parse error)" << std::endl;
        } else {
            std::cout << "✗ (" << result.errorMessage << ")" << std::endl;
        }
    }
}

void BenchmarkReporter::displayBenchmarkHeader(const std::string& provider, const std::string& testPrompt) {
    std::cout << "Running benchmark tests for provider '" << provider << "'..." << std::endl;
    std::cout << "Test prompt: \"" << testPrompt << "\"" << std::endl;
    std::cout << std::endl;
}

void BenchmarkReporter::displayModelCounts(size_t totalModels, size_t blacklistedCount, size_t testingCount) {
    std::cout << "Found " << totalModels << " models total";
    if (blacklistedCount > 0) {
        std::cout << ", filtering out " << blacklistedCount << " blacklisted models";
    }
    std::cout << std::endl;
    std::cout << "Testing " << testingCount << " models:" << std::endl;
}

void BenchmarkReporter::displayNoModelsError(const std::string& provider) {
    std::cerr << "No models found for provider '" << provider << "'" << std::endl;
}

void BenchmarkReporter::displayResults(const std::vector<BenchmarkResult>& results) {
    if (results.empty()) {
        std::cout << "No benchmark results to display." << std::endl;
        return;
    }

    std::cout << std::endl;
    printSeparator();
    std::cout << std::endl;

    // Separate successful and failed results
    std::vector<BenchmarkResult> successful;
    std::vector<BenchmarkResult> failed;
    
    for (const auto& result : results) {
        if (result.success) {
            successful.push_back(result);
        } else {
            failed.push_back(result);
        }
    }

    // Display results sections
    if (!successful.empty()) {
        displaySuccessfulResults(successful);
    }

    if (!failed.empty()) {
        displayFailedResults(failed);
    }

    // Display summary
    displaySummary(results);
}

void BenchmarkReporter::displaySuccessfulResults(const std::vector<BenchmarkResult>& successful) {
    std::cout << "✓ Successful tests (sorted by speed):" << std::endl;
    for (size_t i = 0; i < successful.size(); ++i) {
        const auto& result = successful[i];
        std::cout << formatModelEntry(result, i + 1) << std::endl;
    }
    std::cout << std::endl;
}

void BenchmarkReporter::displayFailedResults(const std::vector<BenchmarkResult>& failed) {
    std::cout << "✗ Failed tests:" << std::endl;
    for (const auto& result : failed) {
        std::cout << result.model << " (" << result.provider << ") - " 
                  << result.errorMessage << std::endl;
    }
    std::cout << std::endl;
}

void BenchmarkReporter::displaySummary(const std::vector<BenchmarkResult>& results) {
    // Count successful and failed results
    size_t successfulCount = 0;
    size_t failedCount = 0;
    const BenchmarkResult* fastest = nullptr;

    for (const auto& result : results) {
        if (result.success) {
            successfulCount++;
            if (fastest == nullptr || result.responseTimeMs < fastest->responseTimeMs) {
                fastest = &result;
            }
        } else {
            failedCount++;
        }
    }

    // Display summary statistics
    std::cout << "Summary: " << successfulCount << " successful, " << failedCount << " failed" << std::endl;
    
    if (fastest != nullptr) {
        std::cout << "Fastest model: " << fastest->model << " (" 
                  << formatResponseTime(fastest->responseTimeMs) << ")" << std::endl;
    }
}

std::string BenchmarkReporter::formatResponseTime(double timeMs) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(BenchmarkConfig::getResponseTimePrecision()) 
        << timeMs << "ms";
    return oss.str();
}

std::string BenchmarkReporter::formatModelEntry(const BenchmarkResult& result, size_t index) {
    std::ostringstream oss;
    if (index > 0) {
        oss << index << ". ";
    }
    oss << result.model << " (" << result.provider << ") - " << formatResponseTime(result.responseTimeMs);
    return oss.str();
}

void BenchmarkReporter::printSeparator() {
    std::cout << "=== BENCHMARK RESULTS ===";
}
