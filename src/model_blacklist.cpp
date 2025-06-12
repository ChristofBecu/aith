#include "model_blacklist.h"
#include "system_utils.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>

/**
 * Checks if a model is blacklisted for a specific provider.
 */
bool ModelBlacklist::isModelBlacklisted(const std::string &provider, const std::string &modelName) {
    std::string blacklistPath = getBlacklistPath();
    
    if (!std::filesystem::exists(blacklistPath)) {
        return false;
    }
    
    std::ifstream blacklistFile(blacklistPath);
    if (!blacklistFile.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(blacklistFile, line)) {
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
    
    return false;
}

/**
 * Adds a model to the blacklist for a specific provider.
 */
void ModelBlacklist::addModelToBlacklist(const std::string &provider, const std::string &modelName, 
                                        const std::string &reason) {
    if (isModelBlacklisted(provider, modelName)) {
        // Model already blacklisted for this provider, no need to add it again
        std::cout << "Model '" << modelName << "' already blacklisted for provider '" << provider << "'." << std::endl;
        return;
    }
    
    ensureConfigDirectoryExists();
    
    std::string blacklistPath = getBlacklistPath();
    
    // Open the blacklist file in append mode
    std::ofstream blacklistFile(blacklistPath, std::ios::app);
    if (!blacklistFile.is_open()) {
        std::cerr << "Error: Could not open blacklist file for writing" << std::endl;
        return;
    }
    
    // Get current timestamp
    std::string timestamp = getCurrentTimestamp();
    
    // Write the entry to the blacklist file in pipe-separated format
    blacklistFile << provider << " | " << modelName << " | ";
    if (!reason.empty()) {
        blacklistFile << reason;
    }
    blacklistFile << " # Added on " << timestamp << std::endl;
    
    blacklistFile.close();
    
    std::cout << "Model '" << modelName << "' added to blacklist for provider '" << provider << "'." << std::endl;
}

/**
 * Removes a model from the blacklist for a specific provider.
 */
void ModelBlacklist::removeModelFromBlacklist(const std::string &provider, const std::string &modelName) {
    std::string blacklistPath = getBlacklistPath();
    
    if (!std::filesystem::exists(blacklistPath)) {
        std::cerr << "Blacklist file does not exist." << std::endl;
        return;
    }
    
    // Read the entire blacklist file
    std::ifstream blacklistFile(blacklistPath);
    if (!blacklistFile.is_open()) {
        std::cerr << "Error: Could not open blacklist file for reading" << std::endl;
        return;
    }
    
    std::vector<std::string> lines;
    std::string line;
    bool modelFound = false;
    
    while (std::getline(blacklistFile, line)) {
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
            lines.push_back(line);
        }
    }
    
    blacklistFile.close();
    
    if (!modelFound) {
        std::cout << "Model '" << modelName << "' not found in blacklist for provider '" << provider << "'." << std::endl;
        return;
    }
    
    // Write the updated blacklist back to the file
    std::ofstream outFile(blacklistPath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open blacklist file for writing" << std::endl;
        return;
    }
    
    for (const auto &l : lines) {
        outFile << l << std::endl;
    }
    
    outFile.close();
    
    std::cout << "Model '" << modelName << "' removed from blacklist for provider '" << provider << "'." << std::endl;
}

/**
 * Returns a list of all blacklisted models and their information.
 */
std::vector<BlacklistEntry> ModelBlacklist::getBlacklistedModels() {
    std::vector<BlacklistEntry> blacklistedModels;
    
    std::string blacklistPath = getBlacklistPath();
    
    if (!std::filesystem::exists(blacklistPath)) {
        return blacklistedModels; // Empty vector
    }
    
    std::ifstream blacklistFile(blacklistPath);
    if (!blacklistFile.is_open()) {
        return blacklistedModels; // Empty vector
    }
    
    std::string line;
    while (std::getline(blacklistFile, line)) {
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
    
    return blacklistedModels;
}

/**
 * Gets the path to the blacklist file (~/.config/ai/blacklist).
 */
std::string ModelBlacklist::getBlacklistPath() {
    std::string home = SystemUtils::getEnvVar("HOME");
    return home + "/.config/ai/blacklist";
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

/**
 * Ensures the config directory exists and creates it if necessary.
 */
void ModelBlacklist::ensureConfigDirectoryExists() {
    std::string home = SystemUtils::getEnvVar("HOME");
    std::string configDir = home + "/.config/ai";
    
    if (!std::filesystem::exists(configDir)) {
        std::filesystem::create_directories(configDir);
    }
}
