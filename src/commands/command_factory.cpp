#include "command_factory.h"
#include "list_command.h"
#include "history_command.h"
#include "chat_command.h"
#include "benchmark_command.h"
#include "blacklist_command.h"
#include "help_command.h"
#include "config_command.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

/**
 * List of all supported commands in the application.
 * This centralized list makes it easy to add new commands
 * and maintain consistency across the application.
 */
static const std::vector<std::string> SUPPORTED_COMMANDS = {
    "list",
    "history", 
    "test",
    "benchmark",  // Alias for "test"
    "blacklist",
    "new",
    "config",
    "help"
    // Note: Direct chat prompts are handled as a special case
};

/**
 * Creates a command instance based on the command name.
 * 
 * This is the main factory method that routes command creation
 * to the appropriate command class constructor.
 */
std::unique_ptr<Command> CommandFactory::createCommand(
    const std::string& commandName,
    const std::vector<std::string>& commandArgs,
    const ApplicationSetup::Config& config) {
    
    validateCommandName(commandName);
    
    if (commandName == "list") {
        return std::make_unique<ListCommand>(
            commandArgs,
            config.apiKey,
            config.provider,
            config.currentHistory,
            config.historyDir
        );
    } else if (commandName == "history") {
        return std::make_unique<HistoryCommand>(
            commandArgs,
            config.apiKey,
            config.provider,
            config.currentHistory,
            config.historyDir
        );
    } else if (commandName == "test" || commandName == "benchmark") {
        return std::make_unique<BenchmarkCommand>(commandArgs, config);
    } else if (commandName == "blacklist") {
        return std::make_unique<BlacklistCommand>(commandArgs, config);
    } else if (commandName == "config") {
        return std::make_unique<ConfigCommand>(commandArgs, config);
    } else if (commandName == "new") {
        return std::make_unique<ChatCommand>(commandArgs, config);
    } else if (commandName == "help") {
        return std::make_unique<HelpCommand>(commandArgs, config);
    } else if (isChatCommand(commandName)) {
        return std::make_unique<ChatCommand>(commandArgs, config);
    } else {
        throw std::invalid_argument("Unknown command: " + commandName);
    }
}

/**
 * Checks if a command name is valid and supported.
 */
bool CommandFactory::isValidCommand(const std::string& commandName) {
    if (commandName.empty()) {
        return false;
    }
    
    // Check if it's in the supported commands list
    auto it = std::find(SUPPORTED_COMMANDS.begin(), SUPPORTED_COMMANDS.end(), commandName);
    if (it != SUPPORTED_COMMANDS.end()) {
        return true;
    }
    
    // If not a known command, treat as potential chat prompt (always valid)
    return true;
}

/**
 * Gets a list of all supported command names.
 */
std::vector<std::string> CommandFactory::getSupportedCommands() {
    std::vector<std::string> commands = SUPPORTED_COMMANDS;
    commands.push_back("chat");  // Add chat as a conceptual command
    return commands;
}

/**
 * Validates that a command name is not empty or invalid.
 */
void CommandFactory::validateCommandName(const std::string& commandName) {
    if (commandName.empty()) {
        throw std::invalid_argument("Command name cannot be empty");
    }
}

/**
 * Determines if the command represents a direct chat prompt.
 * 
 * Only treat as a chat command if it contains spaces (multi-word), 
 * indicating it was likely quoted and is intended as a prompt.
 * Single-word unrecognized commands should be treated as errors.
 */
bool CommandFactory::isChatCommand(const std::string& commandName) {
    // If it's a known command, it's definitely not a chat command
    auto it = std::find(SUPPORTED_COMMANDS.begin(), SUPPORTED_COMMANDS.end(), commandName);
    if (it != SUPPORTED_COMMANDS.end()) {
        return false;
    }
    
    // Only treat as chat command if it contains spaces (multi-word)
    // This implements the requirement that single-word unrecognized commands should error
    return commandName.find(' ') != std::string::npos;
}
