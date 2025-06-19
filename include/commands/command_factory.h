#pragma once

#include "command.h"
#include "core/application_setup.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

/**
 * @brief Factory class for creating command instances.
 * 
 * This class implements the Factory pattern using a functional approach
 * with a map of command names to factory functions. This eliminates
 * code duplication and provides O(1) command creation.
 * 
 * The factory supports all application commands:
 * - list: List available AI models
 * - history: Show conversation history
 * - benchmark: Run model performance tests
 * - blacklist: Manage blacklisted models
 * - config: Manage configuration settings
 * - new: Start new conversation
 * - help: Show help information
 * - chat: Direct chat interaction (for multi-word prompts)
 */
class CommandFactory {
public:
    /**
     * @brief Type alias for command factory functions.
     */
    using CommandCreator = std::function<std::unique_ptr<Command>(
        const std::vector<std::string>&, 
        const ApplicationSetup::Config&)>;

    /**
     * @brief Creates a command instance based on the command name.
     * 
     * Uses a map of factory functions for O(1) command creation.
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
     * @param commandName The command name to validate
     * @return true if the command is supported, false otherwise
     */
    static bool isValidCommand(const std::string& commandName);

    /**
     * @brief Gets a list of all supported command names.
     * 
     * @return Vector containing all supported command names
     */
    static std::vector<std::string> getSupportedCommands();

private:
    /**
     * @brief Gets the static map of command creators.
     * 
     * @return Reference to the map of command name to factory function
     */
    static const std::unordered_map<std::string, CommandCreator>& getCommandCreators();

    /**
     * @brief Determines if the input represents a direct chat prompt.
     * 
     * Only treats multi-word input as chat commands to ensure
     * single-word unrecognized commands show errors.
     * 
     * @param input The input string to check
     * @return true if this should be treated as a chat command
     */
    static bool isChatCommand(const std::string& input);
};
