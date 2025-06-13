#include "commands/history_command.h"
#include <iostream>
#include <stdexcept>
#include <filesystem>

/**
 * @brief Constructs a HistoryCommand with the provided configuration.
 */
HistoryCommand::HistoryCommand(const std::vector<std::string>& commandArgs,
                              const std::string& commandApiKey,
                              const std::string& commandProvider,
                              const std::string& commandCurrentHistory,
                              const std::string& commandHistoryDir)
    : Command(commandArgs, commandApiKey, commandProvider, commandCurrentHistory, commandHistoryDir) {
}

/**
 * @brief Executes the history listing command.
 */
void HistoryCommand::execute() {
    // Validate arguments before execution
    validateArgs();
    
    // List all files in the history directory
    for (const auto& entry : std::filesystem::directory_iterator(historyDir)) {
        std::cout << entry.path().filename().string() << std::endl;
    }
    
    // Display current history file
    std::cout << "\n Current history file: " << currentHistory << std::endl;
}

/**
 * @brief Validates that the command can be executed.
 */
void HistoryCommand::validateArgs() const {
    if (historyDir.empty()) {
        throw std::invalid_argument("History directory path is required");
    }
    
    if (!std::filesystem::exists(historyDir)) {
        throw std::runtime_error("History directory does not exist: " + historyDir);
    }
    
    if (!std::filesystem::is_directory(historyDir)) {
        throw std::runtime_error("History path is not a directory: " + historyDir);
    }
}

/**
 * @brief Gets the command name.
 */
std::string HistoryCommand::getCommandName() const {
    return "history";
}
