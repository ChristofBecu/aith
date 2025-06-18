#include "commands/history_command.h"
#include "directory_operations.h"
#include "file_operations.h"
#include "conversation_displayer.h"
#include <iostream>
#include <stdexcept>
#include <ctime>

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
 * @brief Executes the history command with subcommand support.
 */
void HistoryCommand::execute() {
    // Validate arguments before execution
    validateArgs();
    
    // Parse and execute the appropriate subcommand
    executeSubcommand();
}

/**
 * @brief Validates that the command can be executed.
 */
void HistoryCommand::validateArgs() const {
    if (historyDir.empty()) {
        throw std::invalid_argument("History directory path is required");
    }
    
    if (!DirectoryOperations::exists(historyDir)) {
        throw std::runtime_error("History directory does not exist: " + historyDir);
    }
}

/**
 * @brief Gets the command name.
 */
std::string HistoryCommand::getCommandName() const {
    return "history";
}

/**
 * @brief Parses and executes the appropriate subcommand.
 */
void HistoryCommand::executeSubcommand() {
    // If no arguments provided beyond the command name, default to list behavior
    if (args.size() <= 1) {
        executeList();
        return;
    }
    
    // Parse the subcommand from the second argument (args[1])
    // args[0] is "history", args[1] is the actual subcommand
    const std::string& subcommand = args[1];
    
    if (subcommand == "view") {
        // Determine the target (third argument, or default to "current")
        std::string target = (args.size() > 2) ? args[2] : "current";
        executeView(target);
    } else {
        // Unknown subcommand - show available options
        std::cerr << "Unknown subcommand: " << subcommand << std::endl;
        std::cerr << "Available subcommands:" << std::endl;
        std::cerr << "  (none)  - List all history files" << std::endl;
        std::cerr << "  view    - View conversation content" << std::endl;
        throw std::invalid_argument("Invalid subcommand: " + subcommand);
    }
}

/**
 * @brief Executes the default list behavior (no subcommand).
 */
void HistoryCommand::executeList() {
    // List all files in the history directory using DirectoryOperations
    std::vector<std::string> files = DirectoryOperations::list(historyDir, true);
    for (const auto& filename : files) {
        std::cout << filename << std::endl;
    }
    
    // Display current history file
    std::cout << "\n Current history file: " << currentHistory << std::endl;
}

/**
 * @brief Executes the 'view' subcommand.
 */
void HistoryCommand::executeView(const std::string& target) {
    try {
        // Resolve the target to an actual file path
        std::string filePath = resolveTarget(target);
        
        // Use ConversationDisplayer to show the conversation
        ConversationDisplayer::displayConversation(filePath, true);
        
    } catch (const std::exception& e) {
        std::cerr << "Error viewing history: " << e.what() << std::endl;
        throw;
    }
}

/**
 * @brief Resolves a target string to an actual history file path.
 */
std::string HistoryCommand::resolveTarget(const std::string& target) const {
    if (target == "current" || target == ".") {
        // Return the current history file
        if (currentHistory.empty()) {
            throw std::runtime_error("No current history file is set");
        }
        return currentHistory;
    } else if (target == "latest") {
        // Find the most recently modified file in the history directory
        std::vector<std::string> files = DirectoryOperations::list(historyDir, true);
        if (files.empty()) {
            throw std::runtime_error("No history files found in directory: " + historyDir);
        }
        
        // Filter to only .json files and find the most recent
        std::string latestFile;
        std::time_t latestTime = 0;
        
        for (const auto& filename : files) {
            // Skip non-JSON files
            if (filename.length() < 5 || filename.substr(filename.length() - 5) != ".json") {
                continue;
            }
            
            std::string fullPath = historyDir + "/" + filename;
            try {
                // Get file modification time (for now, use a simple heuristic based on filename)
                // TODO: Implement actual file stat checking for modification time
                
                // For history files with timestamps in name, extract and compare
                if (filename.substr(0, 8) == "history_") {
                    // Look for timestamp pattern _YYYYMMDD_HHMMSS
                    size_t timestampPos = filename.rfind("_20");
                    if (timestampPos != std::string::npos && 
                        timestampPos + 16 <= filename.length()) { // _YYYYMMDD_HHMMSS = 16 chars
                        std::string timestampStr = filename.substr(timestampPos + 1, 15); // YYYYMMDD_HHMMSS
                        
                        // Simple comparison - newer timestamps are lexicographically larger
                        if (latestFile.empty() || timestampStr > latestFile) {
                            latestFile = timestampStr;
                            latestTime = 1; // Mark as found
                        }
                    }
                } else if (filename.substr(0, 8) == "current_") {
                    // Current files are considered "latest"
                    if (latestTime == 0) { // Only if no history file found yet
                        latestFile = filename;
                        latestTime = 1;
                    }
                }
            } catch (const std::exception&) {
                // Skip files that can't be accessed
                continue;
            }
        }
        
        if (latestTime == 0) {
            throw std::runtime_error("No valid history files found in directory: " + historyDir);
        }
        
        // If latestFile contains just timestamp, find the actual filename
        if (latestFile.length() == 15 && latestFile.find("_") == 8) {
            // This is a timestamp, find the file with this timestamp
            for (const auto& filename : files) {
                if (filename.find(latestFile) != std::string::npos) {
                    latestFile = filename;
                    break;
                }
            }
        }
        
        return historyDir + "/" + latestFile;
    } else {
        // Treat as a filename or full path
        std::string fullPath;
        
        // Check if it's already a full path (starts with /)
        if (target[0] == '/') {
            fullPath = target;
        } else {
            // Relative path - construct full path with history directory
            fullPath = historyDir + "/" + target;
        }
        
        // Check if the file exists
        if (!FileOperations::exists(fullPath)) {
            throw std::runtime_error("History file not found: " + target);
        }
        
        return fullPath;
    }
}
