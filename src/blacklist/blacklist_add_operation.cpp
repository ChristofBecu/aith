#include "blacklist_add_operation.h"
#include "blacklist_file_manager.h"
#include "blacklist_parser.h"
#include "blacklist_check_operation.h"
#include <iostream>
#include <stdexcept>

/**
 * Constructs a blacklist add operation.
 */
BlacklistAddOperation::BlacklistAddOperation(const std::string& provider, 
                                           const std::string& model,
                                           const std::string& reason,
                                           std::shared_ptr<BlacklistFileManager> fileManager,
                                           std::shared_ptr<BlacklistParser> parser)
    : BlacklistOperation(provider, model, fileManager, parser)
    , reason_(reason)
    , successful_(false)
    , executed_(false) {
}

/**
 * Executes the add operation.
 */
void BlacklistAddOperation::execute() {
    // Validate inputs before execution
    validateInputs();
    
    try {
        // Check if model is already blacklisted
        if (isAlreadyBlacklisted()) {
            std::cout << "Model '" << model_ << "' already blacklisted for provider '" 
                      << provider_ << "'." << std::endl;
            successful_ = false;
            executed_ = true;
            return;
        }
        
        // Perform the actual add operation
        performAdd();
        
        // Success feedback
        std::cout << "Model '" << model_ << "' added to blacklist for provider '" 
                  << provider_ << "'." << std::endl;
        successful_ = true;
        executed_ = true;
        
    } catch (const std::exception& e) {
        // Error feedback
        std::cerr << "Error: Could not add model to blacklist - " << e.what() << std::endl;
        successful_ = false;
        executed_ = true;
    }
}

/**
 * Validates inputs specific to add operations.
 */
void BlacklistAddOperation::validateInputs() const {
    // Validate provider and model using base class
    validateProviderAndModel();
    
    // Validate reason specific to add operations
    validateReason();
}

/**
 * Returns the name of this operation.
 */
std::string BlacklistAddOperation::getOperationName() const {
    return "add";
}

/**
 * Checks if the operation completed successfully.
 */
bool BlacklistAddOperation::wasSuccessful() const {
    if (!executed_) {
        throw std::logic_error("Cannot get success status: execute() has not been called yet");
    }
    return successful_;
}

/**
 * Checks if the operation has been executed.
 */
bool BlacklistAddOperation::hasBeenExecuted() const {
    return executed_;
}

/**
 * Checks if the model is already blacklisted.
 */
bool BlacklistAddOperation::isAlreadyBlacklisted() {
    // Create a check operation to determine if already blacklisted
    BlacklistCheckOperation checkOp(provider_, model_, fileManager_, parser_);
    checkOp.execute();
    return checkOp.isBlacklisted();
}

/**
 * Performs the actual add operation.
 */
void BlacklistAddOperation::performAdd() {
    // Create the formatted entry line
    std::string entryLine = BlacklistParser::formatEntry(provider_, model_, reason_);
    
    try {
        // Append the entry to the blacklist file
        fileManager_->appendLine(entryLine);
    } catch (const std::runtime_error& e) {
        // Re-throw with more context
        throw std::runtime_error("Failed to write to blacklist file: " + std::string(e.what()));
    }
}

/**
 * Validates that the reason is not empty and meaningful.
 */
void BlacklistAddOperation::validateReason() const {
    if (reason_.empty()) {
        throw std::invalid_argument("Reason cannot be empty for add operation");
    }
    
    // Check for whitespace-only reason
    if (BlacklistParser::trimWhitespace(reason_).empty()) {
        throw std::invalid_argument("Reason cannot be whitespace-only for add operation");
    }
    
    // Ensure reason is not too long (reasonable limit)
    if (reason_.length() > 500) {
        throw std::invalid_argument("Reason is too long (maximum 500 characters)");
    }
}
