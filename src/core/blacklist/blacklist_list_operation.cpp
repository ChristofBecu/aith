#include "blacklist_list_operation.h"
#include "blacklist_file_manager.h"
#include "blacklist_parser.h"
#include <iostream>

/**
 * @brief Constructs a BlacklistListOperation with required dependencies.
 */
BlacklistListOperation::BlacklistListOperation(std::shared_ptr<BlacklistFileManager> fileManager, 
                                              std::shared_ptr<BlacklistParser> parser)
    : BlacklistOperation("", "", fileManager, parser) {  // Empty provider/model for list operation
    // Initialize empty vector
    blacklistedModels_.clear();
}

/**
 * @brief Executes the list operation.
 */
void BlacklistListOperation::execute() {
    // Clear any previous results
    blacklistedModels_.clear();
    
    // Check if blacklist file exists
    if (!fileManager_->exists()) {
        // File doesn't exist - return empty list (not an error)
        return;
    }
    
    try {
        // Read all lines from the blacklist file
        std::vector<std::string> lines = fileManager_->readAllLines();
        
        // Parse each line and add valid entries to our result vector
        for (const std::string& line : lines) {
            // Parse the line using the parser
            ParsedBlacklistEntry parsedEntry = BlacklistParser::parseLine(line);
            
            // Create BlacklistEntry if we have a valid parsed entry
            if (parsedEntry.isValid) {
                BlacklistEntry entry;
                entry.provider = parsedEntry.provider;
                entry.model = parsedEntry.model;
                entry.reason = parsedEntry.reason;
                entry.timestamp = parsedEntry.timestamp;
                
                blacklistedModels_.push_back(entry);
            }
        }
        
    } catch (const std::runtime_error& e) {
        // Log the error but don't crash - list operation should be resilient
        std::cerr << "Error reading blacklist file during list operation: " << e.what() << std::endl;
        // Leave blacklistedModels_ empty as a fallback
    }
}

/**
 * @brief Gets the list of blacklisted models after execution.
 */
std::vector<BlacklistEntry> BlacklistListOperation::getBlacklistedModels() const {
    return blacklistedModels_;
}

/**
 * @brief Validates inputs for the list operation.
 */
void BlacklistListOperation::validateInputs() const {
    // The list operation doesn't require any specific inputs,
    // so validation always succeeds. Dependencies are validated
    // by the base class constructor.
}

/**
 * @brief Gets the operation name for logging and debugging.
 */
std::string BlacklistListOperation::getOperationName() const {
    return "list";
}
