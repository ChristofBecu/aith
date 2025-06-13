#include "commands/benchmark_command.h"
#include "benchmark/benchmark.h"
#include <iostream>
#include <stdexcept>

// Default test prompt for benchmarking
const std::string BenchmarkCommand::DEFAULT_TEST_PROMPT = "Hello";

BenchmarkCommand::BenchmarkCommand(const std::vector<std::string>& args, const ApplicationSetup::Config& config)
    : Command(args, config.apiKey, config.provider, config.currentHistory, config.historyDir),
      args_(args), config_(config) {
}

void BenchmarkCommand::execute() {
    validateArgs();
    
    std::string testPrompt = getTestPrompt();
    
    std::cout << "Starting model benchmark tests..." << std::endl;
    
    try {
        auto results = runAllModelsBenchmark(config_.apiKey, testPrompt);
        displayBenchmarkResults(results);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to run benchmark tests: " + std::string(e.what()));
    }
}

void BenchmarkCommand::validateArgs() const {
    // The benchmark command is valid with or without additional arguments
    // If arguments are provided, they should be the custom test prompt
    if (args_.empty()) {
        throw std::invalid_argument("Benchmark command requires at least the command name argument");
    }
    
    std::string command = args_[0];
    if (command != "test" && command != "benchmark") {
        throw std::invalid_argument("Invalid benchmark command: " + command);
    }
}

std::string BenchmarkCommand::getCommandName() const {
    if (!args_.empty() && args_[0] == "benchmark") {
        return "benchmark";
    }
    return "test";
}

std::string BenchmarkCommand::getTestPrompt() const {
    // If a custom test prompt was provided, use it
    if (args_.size() > 1) {
        return args_[1];
    }
    
    // Otherwise, use the default test prompt
    return DEFAULT_TEST_PROMPT;
}
