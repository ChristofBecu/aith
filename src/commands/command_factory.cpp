#include "command_factory.h"
#include "list_command.h"
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
    "new"
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
        throw std::runtime_error("HistoryCommand implementation not yet available - Phase 6");
    } else if (commandName == "test" || commandName == "benchmark") {
        throw std::runtime_error("BenchmarkCommand implementation not yet available - Phase 9");
    } else if (commandName == "blacklist") {
        throw std::runtime_error("BlacklistCommand implementation not yet available - Phase 10");
    } else if (commandName == "new") {
        throw std::runtime_error("NewCommand implementation not yet available - Phase 8");
    } else if (isChatCommand(commandName)) {
        throw std::runtime_error("ChatCommand implementation not yet available - Phase 7");
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
 * Any command that doesn't match the known command list is treated
 * as a direct chat prompt, following the application's default behavior.
 */
bool CommandFactory::isChatCommand(const std::string& commandName) {
    auto it = std::find(SUPPORTED_COMMANDS.begin(), SUPPORTED_COMMANDS.end(), commandName);
    return it == SUPPORTED_COMMANDS.end();
}
