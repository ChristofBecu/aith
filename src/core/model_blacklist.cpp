#include "model_blacklist.h"
#include "blacklist/blacklist_file_manager.h"
#include "blacklist/blacklist_parser.h"
#include "blacklist/blacklist_operation_factory.h"
#include "blacklist/blacklist_check_operation.h"
#include <iostream>
#include <algorithm>

/**
 * Gets the file manager instance for blacklist operations.
 */
BlacklistFileManager& ModelBlacklist::getFileManager() {
    static BlacklistFileManager instance;
    return instance;
}

/**
 * Checks if a model is blacklisted for a specific provider.
 */
bool ModelBlacklist::isModelBlacklisted(const std::string &provider, const std::string &modelName) {
    try {
        // Use the factory to create a BlacklistCheckOperation
        auto checkOperation = BlacklistOperationFactory::createOperation(
            BlacklistOperationFactory::OperationType::CHECK, provider, modelName);
        
        // Execute the check
        checkOperation->execute();
        
        // Cast to BlacklistCheckOperation to get the result
        auto* checkOp = dynamic_cast<BlacklistCheckOperation*>(checkOperation.get());
        if (checkOp) {
            return checkOp->isBlacklisted();
        }
        
        // Fallback - should not happen in normal operation
        std::cerr << "Warning: Failed to cast to BlacklistCheckOperation" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        // Log error but don't crash - blacklist check should be resilient
        std::cerr << "Error checking blacklist for " << provider << "/" << modelName 
                  << ": " << e.what() << std::endl;
        return false;
    }
}

/**
 * Adds a model to the blacklist for a specific provider.
 */
void ModelBlacklist::addModelToBlacklist(const std::string &provider, const std::string &modelName, 
                                        const std::string &reason) {
    try {
        // Use the factory to create a BlacklistAddOperation
        auto addOperation = BlacklistOperationFactory::createOperation(
            BlacklistOperationFactory::OperationType::ADD, provider, modelName, reason);
        
        // Execute the add operation
        addOperation->execute();
        
    } catch (const std::exception& e) {
        // Log error but don't crash - add operation should handle most errors internally
        std::cerr << "Error in addModelToBlacklist: " << e.what() << std::endl;
    }
}

/**
 * Removes a model from the blacklist for a specific provider.
 */
void ModelBlacklist::removeModelFromBlacklist(const std::string &provider, const std::string &modelName) {
    BlacklistFileManager& fileManager = getFileManager();
    
    if (!fileManager.exists()) {
        std::cout << "Blacklist file does not exist." << std::endl;
        return;
    }
    
    try {
        // Read the entire blacklist file
        std::vector<std::string> lines = fileManager.readAllLines();
        std::vector<std::string> filteredLines;
        bool modelFound = false;
        
        for (const std::string& line : lines) {
            bool keepLine = true;
            
            // Skip empty lines and comments, but keep them in the file
            if (!BlacklistParser::isEmptyLine(line) && !BlacklistParser::isCommentLine(line)) {
                // Parse the line  
                ParsedBlacklistEntry entry = BlacklistParser::parseLine(line);
                
                // Check if we have a valid entry and it matches
                if (entry.isValid && entry.provider == provider && entry.model == modelName) {
                    keepLine = false;
                    modelFound = true;
                }
            }
            
            if (keepLine) {
                filteredLines.push_back(line);
            }
        }
        
        if (!modelFound) {
            std::cout << "Model '" << modelName << "' not found in blacklist for provider '" << provider << "'." << std::endl;
            return;
        }
        
        // Write the updated blacklist back to the file
        fileManager.writeAllLines(filteredLines);
        std::cout << "Model '" << modelName << "' removed from blacklist for provider '" << provider << "'." << std::endl;
        
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: Could not remove model from blacklist - " << e.what() << std::endl;
    }
}

/**
 * Returns a list of all blacklisted models and their information.
 */
std::vector<BlacklistEntry> ModelBlacklist::getBlacklistedModels() {
    std::vector<BlacklistEntry> blacklistedModels;
    BlacklistFileManager& fileManager = getFileManager();
    
    if (!fileManager.exists()) {
        return blacklistedModels; // Empty vector
    }
    
    try {
        std::vector<std::string> lines = fileManager.readAllLines();
        
        for (const std::string& line : lines) {
            // Parse the line using BlacklistParser
            ParsedBlacklistEntry parsedEntry = BlacklistParser::parseLine(line);
            
            // Create BlacklistEntry if we have a valid parsed entry
            if (parsedEntry.isValid) {
                BlacklistEntry entry;
                entry.provider = parsedEntry.provider;
                entry.model = parsedEntry.model;
                entry.reason = parsedEntry.reason;
                entry.timestamp = parsedEntry.timestamp;
                
                blacklistedModels.push_back(entry);
            }
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error reading blacklist file: " << e.what() << std::endl;
    }
    
    return blacklistedModels;
}
