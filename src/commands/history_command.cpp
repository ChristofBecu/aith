#include "commands/history_command.h"
#include "file_utils.h"
#include <iostream>
#include <stdexcept>

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
    
    // List all files in the history directory using FileUtils
    std::vector<std::string> files = FileUtils::listDirectory(historyDir, true);
    for (const auto& filename : files) {
        std::cout << filename << std::endl;
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
    
    if (!FileUtils::directoryExists(historyDir)) {
        throw std::runtime_error("History directory does not exist: " + historyDir);
    }
}

/**
 * @brief Gets the command name.
 */
std::string HistoryCommand::getCommandName() const {
    return "history";
}
