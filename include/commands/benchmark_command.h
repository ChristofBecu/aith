#ifndef BENCHMARK_COMMAND_H
#define BENCHMARK_COMMAND_H

#include "command.h"
#include "application_setup.h"

/**
 * @class BenchmarkCommand
 * @brief Command for running model benchmark tests
 * 
 * This command handles the execution of benchmark tests on AI models.
 * It supports both "test" and "benchmark" as command aliases and allows
 * for custom test prompts to be specified.
 */
class BenchmarkCommand : public Command {
public:
    /**
     * @brief Constructor for BenchmarkCommand
     * @param args Command line arguments
     * @param config Application configuration
     */
    BenchmarkCommand(const std::vector<std::string>& args, const ApplicationSetup::Config& config);
    
    /**
     * @brief Execute the benchmark command
     * 
     * This method runs benchmark tests on all available models using either
     * a default test prompt or a custom prompt provided by the user.
     */
    void execute() override;
    
    /**
     * @brief Validates the command arguments
     * 
     * Ensures that the benchmark command arguments are valid.
     */
    void validateArgs() const override;
    
    /**
     * @brief Gets the command name for identification
     * 
     * @return The command name ("test" or "benchmark")
     */
    std::string getCommandName() const override;

private:
    std::vector<std::string> args_;
    ApplicationSetup::Config config_;
    
    /**
     * @brief Gets the test prompt to use for benchmarking
     * 
     * @return The test prompt (default or user-specified)
     */
    std::string getTestPrompt() const;
    
    /**
     * @brief Default test prompt used when no custom prompt is provided
     */
    static const std::string DEFAULT_TEST_PROMPT;
};

#endif // BENCHMARK_COMMAND_H
