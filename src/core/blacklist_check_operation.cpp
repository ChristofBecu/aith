#include "blacklist_check_operation.h"
#include "blacklist_file_manager.h"
#include "blacklist_parser.h"
#include <iostream>
#include <stdexcept>

/**
 * Constructs a blacklist check operation.
 */
BlacklistCheckOperation::BlacklistCheckOperation(const std::string& provider, 
                                                const std::string& model,
                                                std::shared_ptr<BlacklistFileManager> fileManager,
                                                std::shared_ptr<BlacklistParser> parser)
    : BlacklistOperation(provider, model, fileManager, parser)
    , result_(false)
    , executed_(false) {
}

/**
 * Executes the blacklist check operation.
 */
void BlacklistCheckOperation::execute() {
    // Validate inputs before execution
    validateInputs();
    
    try {
        // Perform the actual check
        result_ = performCheck();
        executed_ = true;
    } catch (const std::runtime_error& e) {
        // Log error but don't re-throw - blacklist check should be resilient
        std::cerr << "Warning: Error during blacklist check for " 
                  << provider_ << "/" << model_ << ": " << e.what() << std::endl;
        result_ = false;  // Default to not blacklisted on error
        executed_ = true;
    }
}

/**
 * Validates the inputs for this operation.
 */
void BlacklistCheckOperation::validateInputs() const {
    validateProviderAndModel();
}

/**
 * Returns the name of this operation.
 */
std::string BlacklistCheckOperation::getOperationName() const {
    return "check";
}

/**
 * Returns the result of the blacklist check.
 */
bool BlacklistCheckOperation::isBlacklisted() const {
    if (!executed_) {
        throw std::logic_error("Cannot get result: execute() has not been called yet");
    }
    return result_;
}

/**
 * Checks if the operation has been executed.
 */
bool BlacklistCheckOperation::hasBeenExecuted() const {
    return executed_;
}

/**
 * Performs the actual blacklist check logic.
 */
bool BlacklistCheckOperation::performCheck() {
    // Check if blacklist file exists
    if (!fileManager_->exists()) {
        return false;  // No blacklist file means nothing is blacklisted
    }
    
    try {
        // Read all lines from the blacklist file
        std::vector<std::string> lines = fileManager_->readAllLines();
        
        // Check each line for a match
        for (const std::string& line : lines) {
            // Skip empty lines and comments
            if (BlacklistParser::isEmptyLine(line) || BlacklistParser::isCommentLine(line)) {
                continue;
            }
            
            // Parse the line
            ParsedBlacklistEntry entry = BlacklistParser::parseLine(line);
            
            // Check if this entry matches our target provider/model
            if (entryMatches(entry)) {
                return true;  // Found a match - model is blacklisted
            }
        }
    } catch (const std::runtime_error& e) {
        // Re-throw file operation errors
        throw std::runtime_error("Failed to read blacklist file: " + std::string(e.what()));
    }
    
    return false;  // No match found - model is not blacklisted
}

/**
 * Checks if a parsed entry matches the target provider/model.
 */
bool BlacklistCheckOperation::entryMatches(const ParsedBlacklistEntry& entry) const {
    return entry.isValid && 
           entry.provider == provider_ && 
           entry.model == model_;
}
