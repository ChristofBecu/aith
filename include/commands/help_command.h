#pragma once

#include "command.h"
#include "core/application_setup.h"

/**
 * @brief Command to display help information for the AITH application.
 * 
 * Displays comprehensive usage information including all available commands,
 * options, examples, and provider information.
 */
class HelpCommand : public Command {
public:
    /**
     * @brief Constructs a help command instance.
     * @param args Command arguments (unused for help)
     * @param config Application configuration
     */
    HelpCommand(const std::vector<std::string>& args, const ApplicationSetup::Config& config);

    /**
     * @brief Executes the help command by displaying usage information.
     */
    void execute() override;

    /**
     * @brief Validates command arguments (help command doesn't require validation).
     */
    void validateArgs() const override;

    /**
     * @brief Gets the command name.
     * @return "help"
     */
    std::string getCommandName() const override;

private:
    /**
     * @brief Displays the main help message with all commands and options.
     */
    void displayMainHelp() const;
    
    /**
     * @brief Displays examples of common usage patterns.
     */
    void displayExamples() const;
    
    /**
     * @brief Displays information about available AI providers.
     */
    void displayProviderInfo() const;
    
    /**
     * @brief Displays information about history and conversation management.
     */
    void displayHistoryInfo() const;

private:
    ApplicationSetup::Config config_;
};
