#pragma once

#include "command.h"
#include "application_setup.h"
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Factory class for creating command instances.
 * 
 * This class implements the Factory pattern to create appropriate
 * command objects based on command names. It centralizes command
 * creation logic and ensures all commands are properly initialized
 * with the required configuration data.
 * 
 * The factory supports all application commands:
 * - list: List available AI models
 * - history: Show conversation history
 * - test/benchmark: Run model performance tests
 * - blacklist: Manage blacklisted models
 * - new: Start new conversation
 * - chat: Direct chat interaction (default)
 */
class CommandFactory {
public:
    /**
     * @brief Creates a command instance based on the command name.
     * 
     * This method analyzes the command name and creates the appropriate
     * command object, initialized with the provided configuration and
     * arguments.
     * 
     * @param commandName The name of the command to create
     * @param commandArgs Arguments specific to the command (excluding command name)
     * @param config Application configuration from ApplicationSetup
     * @return Unique pointer to the created command instance
     * @throws std::invalid_argument if command name is not recognized
     */
    static std::unique_ptr<Command> createCommand(
        const std::string& commandName,
        const std::vector<std::string>& commandArgs,
        const ApplicationSetup::Config& config
    );

    /**
     * @brief Checks if a command name is valid and supported.
     * 
     * This method can be used to validate command names before
     * attempting to create command instances.
     * 
     * @param commandName The command name to validate
     * @return true if the command is supported, false otherwise
     */
    static bool isValidCommand(const std::string& commandName);

    /**
     * @brief Gets a list of all supported command names.
     * 
     * Useful for generating help text or validating user input.
     * 
     * @return Vector containing all supported command names
     */
    static std::vector<std::string> getSupportedCommands();

private:
    /**
     * @brief Validates that a command name is not empty or invalid.
     * 
     * @param commandName The command name to validate
     * @throws std::invalid_argument if command name is empty or invalid
     */
    static void validateCommandName(const std::string& commandName);

    /**
     * @brief Determines if the command represents a direct chat prompt.
     * 
     * If the command name doesn't match any known commands, it's treated
     * as a direct chat prompt (the default behavior).
     * 
     * @param commandName The command name to check
     * @return true if this should be treated as a chat command
     */
    static bool isChatCommand(const std::string& commandName);
};
