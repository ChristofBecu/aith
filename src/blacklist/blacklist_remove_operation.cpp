#include "blacklist_remove_operation.h"
#include "blacklist_file_manager.h"
#include "blacklist_parser.h"
#include <iostream>
#include <stdexcept>

/**
 * Constructs a blacklist remove operation.
 */
BlacklistRemoveOperation::BlacklistRemoveOperation(const std::string& provider,
                                                 const std::string& model,
                                                 std::shared_ptr<BlacklistFileManager> fileManager,
                                                 std::shared_ptr<BlacklistParser> parser)
    : BlacklistOperation(provider, model, fileManager, parser)
    , successful_(false)
    , executed_(false)
    , modelFound_(false) {
}

/**
 * Executes the remove operation.
 */
void BlacklistRemoveOperation::execute() {
    // Validate inputs before execution
    validateInputs();
    
    try {
        // Check if blacklist file exists
        if (!checkFileExists()) {
            std::cout << "Blacklist file does not exist." << std::endl;
            successful_ = false;
            executed_ = true;
            return;
        }
        
        // Perform the actual remove operation
        performRemove();
        
        if (modelFound_) {
            // Success feedback
            std::cout << "Model '" << model_ << "' removed from blacklist for provider '" 
                      << provider_ << "'." << std::endl;
            successful_ = true;
        } else {
            // Model not found feedback
            std::cout << "Model '" << model_ << "' not found in blacklist for provider '" 
                      << provider_ << "'." << std::endl;
            successful_ = false;
        }
        
        executed_ = true;
        
    } catch (const std::exception& e) {
        // Error feedback
        std::cerr << "Error: Could not remove model from blacklist - " << e.what() << std::endl;
        successful_ = false;
        executed_ = true;
    }
}

/**
 * Validates inputs specific to remove operations.
 */
void BlacklistRemoveOperation::validateInputs() const {
    // Validate provider and model using base class
    // Note: Remove operations don't need a reason parameter
    validateProviderAndModel();
}

/**
 * Returns the name of this operation.
 */
std::string BlacklistRemoveOperation::getOperationName() const {
    return "remove";
}

/**
 * Checks if the operation completed successfully.
 */
bool BlacklistRemoveOperation::wasSuccessful() const {
    if (!executed_) {
        throw std::logic_error("Cannot get success status: execute() has not been called yet");
    }
    return successful_;
}

/**
 * Checks if the operation has been executed.
 */
bool BlacklistRemoveOperation::hasBeenExecuted() const {
    return executed_;
}

/**
 * Checks if the target model was found in the blacklist.
 */
bool BlacklistRemoveOperation::wasModelFound() const {
    if (!executed_) {
        throw std::logic_error("Cannot get model found status: execute() has not been called yet");
    }
    return modelFound_;
}

/**
 * Checks if the blacklist file exists.
 */
bool BlacklistRemoveOperation::checkFileExists() {
    return fileManager_->exists();
}

/**
 * Performs the actual remove operation.
 */
void BlacklistRemoveOperation::performRemove() {
    try {
        // Read the entire blacklist file
        std::vector<std::string> lines = fileManager_->readAllLines();
        
        // Filter out the target entry
        std::vector<std::string> filteredLines = filterOutTargetEntry(lines);
        
        // Only rewrite the file if we found and removed something
        if (modelFound_) {
            fileManager_->writeAllLines(filteredLines);
        }
        
    } catch (const std::runtime_error& e) {
        // Re-throw with more context
        throw std::runtime_error("Failed to process blacklist file: " + std::string(e.what()));
    }
}

/**
 * Filters out the target entry from the given lines.
 */
std::vector<std::string> BlacklistRemoveOperation::filterOutTargetEntry(const std::vector<std::string>& lines) {
    std::vector<std::string> filteredLines;
    modelFound_ = false;
    
    for (const std::string& line : lines) {
        bool keepLine = true;
        
        // Skip empty lines and comments, but keep them in the file
        if (!BlacklistParser::isEmptyLine(line) && !BlacklistParser::isCommentLine(line)) {
            // Parse the line
            ParsedBlacklistEntry entry = BlacklistParser::parseLine(line);
            
            // Check if this is the target entry to remove
            if (isTargetEntry(entry)) {
                keepLine = false;
                modelFound_ = true;
            }
        }
        
        // Keep all lines except the target entry
        if (keepLine) {
            filteredLines.push_back(line);
        }
    }
    
    return filteredLines;
}

/**
 * Checks if a parsed entry matches the target provider/model.
 */
bool BlacklistRemoveOperation::isTargetEntry(const ParsedBlacklistEntry& entry) const {
    return entry.isValid && 
           entry.provider == provider_ && 
           entry.model == model_;
}
