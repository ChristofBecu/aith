#include "model_blacklist.h"
#include "blacklist_file_manager.h"
#include <iostream>
#include <sstream>
#include <ctime>
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
    BlacklistFileManager& fileManager = getFileManager();
    
    if (!fileManager.exists()) {
        return false;
    }
    
    try {
        std::vector<std::string> lines = fileManager.readAllLines();
        
        for (const std::string& line : lines) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse the line
            std::vector<std::string> parts = parseBlacklistLine(line);
            
            // Check if we have at least provider and model and they match
            if (parts.size() >= 2 && parts[0] == provider && parts[1] == modelName) {
                return true;
            }
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error reading blacklist file: " << e.what() << std::endl;
        return false;
    }
    
    return false;
}

/**
 * Adds a model to the blacklist for a specific provider.
 */
void ModelBlacklist::addModelToBlacklist(const std::string &provider, const std::string &modelName, 
                                        const std::string &reason) {
    if (ModelBlacklist::isModelBlacklisted(provider, modelName)) {
        // Model already blacklisted for this provider, no need to add it again
        std::cout << "Model '" << modelName << "' already blacklisted for provider '" << provider << "'." << std::endl;
        return;
    }
    
    BlacklistFileManager& fileManager = getFileManager();
    
    // Get current timestamp
    std::string timestamp = getCurrentTimestamp();
    
    // Create the entry line in pipe-separated format
    std::string entryLine = provider + " | " + modelName + " | ";
    if (!reason.empty()) {
        entryLine += reason;
    }
    entryLine += " # Added on " + timestamp;
    
    try {
        fileManager.appendLine(entryLine);
        std::cout << "Model '" << modelName << "' added to blacklist for provider '" << provider << "'." << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: Could not add model to blacklist - " << e.what() << std::endl;
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
            
            // Skip empty lines and comments
            if (!line.empty() && line[0] != '#') {
                // Parse the line
                std::vector<std::string> parts = parseBlacklistLine(line);
                
                // Check if we have at least provider and model and they match
                if (parts.size() >= 2 && parts[0] == provider && parts[1] == modelName) {
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
        
        for (std::string line : lines) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Extract comment and timestamp if present
            std::string timestamp;
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                std::string commentPart = line.substr(commentPos + 1);
                timestamp = extractTimestamp(commentPart);
                // Remove comment part from line for parsing
                line = line.substr(0, commentPos);
            }
            
            // Parse the line
            std::vector<std::string> parts = parseBlacklistLine(line);
            
            // Create BlacklistEntry if we have at least provider and model
            if (parts.size() >= 2) {
                BlacklistEntry entry;
                entry.provider = parts[0];
                entry.model = parts[1];
                
                // Add reason if present
                if (parts.size() >= 3) {
                    entry.reason = parts[2];
                }
                
                entry.timestamp = timestamp;
                
                blacklistedModels.push_back(entry);
            }
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error reading blacklist file: " << e.what() << std::endl;
    }
    
    return blacklistedModels;
}

/**
 * Parses a blacklist line into provider, model, and reason components.
 */
std::vector<std::string> ModelBlacklist::parseBlacklistLine(const std::string &line) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string part;
    
    while (std::getline(ss, part, '|')) {
        // Trim whitespace
        part = trimWhitespace(part);
        parts.push_back(part);
    }
    
    return parts;
}

/**
 * Trims whitespace from the beginning and end of a string.
 */
std::string ModelBlacklist::trimWhitespace(const std::string &str) {
    size_t start = str.find_first_not_of(" \t");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

/**
 * Extracts timestamp from a comment line.
 */
std::string ModelBlacklist::extractTimestamp(const std::string &commentPart) {
    size_t addedOnPos = commentPart.find("Added on ");
    if (addedOnPos != std::string::npos) {
        std::string timestamp = commentPart.substr(addedOnPos + 9); // "Added on " is 9 chars
        return trimWhitespace(timestamp);
    }
    return "";
}

/**
 * Generates a current timestamp string.
 */
std::string ModelBlacklist::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::string timestamp = std::ctime(&now);
    // Remove the newline character from ctime output
    if (!timestamp.empty() && timestamp.back() == '\n') {
        timestamp.pop_back();
    }
    return timestamp;
}
