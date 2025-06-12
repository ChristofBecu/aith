#include "utils.h"
#include "system_utils.h"
#include "config_manager.h"
#include <cstdlib>
#include <stdexcept>
#include <array>
#include <memory>
#include <functional>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <vector>

// Backward compatibility wrappers (deprecated)
std::string getEnvVar(const std::string &key) {
    return SystemUtils::getEnvVar(key);
}

std::string exec(const char* cmd) {
    return SystemUtils::exec(cmd);
}

void executeCommand(const std::string &command) {
    SystemUtils::executeCommand(command);
}

std::string getConfigValue(const std::string &key) {
    return ConfigManager::getConfigValue(key);
}

std::string getProviderConfigValue(const std::string &provider, const std::string &key) {
    return ConfigManager::getProviderConfigValue(provider, key);
}

/**
 * Gets the default model from the provider's config file.
 * @return The default model name.
 */
std::string getDefaultModel() {
    std::string provider = getAgent();
    std::string model = ConfigManager::getProviderConfigValue(provider, "DEFAULT_MODEL");
    if (model.empty()) {
        // Fall back to main config file
        model = ConfigManager::getConfigValue("DEFAULT_MODEL");
    }
    return model;
}

/**
 * Gets the API URL from the provider's config file.
 * @return The API URL.
 */
std::string getApiUrl() {
    std::string provider = getAgent();
    std::string url = ConfigManager::getProviderConfigValue(provider, "API_URL");
    if (url.empty()) {
        // Fall back to main config file
        url = ConfigManager::getConfigValue("API_URL");
    }
    return url;
}

// Global variable to store the provider specified via command line
static std::string commandLineProvider = "";

/**
 * Sets the provider from command line arguments.
 * @param provider The provider specified via command line.
 */
void setCommandLineProvider(const std::string &provider) {
    commandLineProvider = provider;
}

/**
 * Gets the agent/provider type to use.
 * Priority order:
 * 1. Command line specified provider
 * 2. AGENT environment variable
 * 3. Default provider from config file
 * @return The agent/provider type
 */
std::string getAgent() {
    // First check if provider was specified via command line
    if (!commandLineProvider.empty()) {
        return commandLineProvider;
    }
    
    // Then check environment variable
    std::string agent = SystemUtils::getEnvVar("AGENT");
    if (!agent.empty()) {
        return agent;
    }
    
    // Then check the config file
    agent = ConfigManager::getConfigValue("AGENT");
    if (!agent.empty()) {
        return agent;
    }
    
    // Finally, fall back to the default provider
    return getDefaultProvider();
}

/**
 * Gets the API key from the provider's config file.
 * Checks for the API key in the following order:
 * 1. Environment variable with provider prefix (e.g., GROQ_API_KEY)
 * 2. Provider-specific config file (~/.config/ai/provider.conf)
 * 3. Returns empty string if not found
 * @return The API key.
 */
std::string getApiKey() {
    std::string provider = getAgent();
    
    // Try agent-specific environment variable
    std::string envVarName = provider + "_API_KEY";
    std::string apiKey = SystemUtils::getEnvVar(envVarName);
    if (!apiKey.empty()) {
        return apiKey;
    }
    
    // For backward compatibility, try GROQ_API_KEY
    if (provider == "GROQ" || provider == "groq") {
        apiKey = SystemUtils::getEnvVar("GROQ_API_KEY");
        if (!apiKey.empty()) {
            return apiKey;
        }
    }
    
    // Read from provider-specific config file
    apiKey = ConfigManager::getProviderConfigValue(provider, "API_KEY");
    if (!apiKey.empty()) {
        return apiKey;
    }
    
    // Try generic API_KEY from main config file as last resort
    return ConfigManager::getConfigValue("API_KEY");
}

/**
 * Gets the default provider from the main config file.
 * @return The default provider name (defaults to "groq" if not specified)
 */
std::string getDefaultProvider() {
    std::string provider = ConfigManager::getConfigValue("DEFAULT_PROVIDER");
    return provider.empty() ? "groq" : provider;
}

/**
 * Checks if a model is blacklisted for a specific provider.
 * @param provider The provider name.
 * @param modelName The name of the model to check.
 * @return True if the model is blacklisted for the provider, false otherwise.
 */
bool isModelBlacklisted(const std::string &provider, const std::string &modelName) {
    std::string home = SystemUtils::getEnvVar("HOME");
    std::string blacklistPath = home + "/.config/ai/blacklist";
    
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
        
        // Remove any comment portion
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Parse the line with pipe-separated values
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string part;
        
        while (std::getline(ss, part, '|')) {
            // Trim whitespace
            part.erase(0, part.find_first_not_of(" \t"));
            part.erase(part.find_last_not_of(" \t") + 1);
            parts.push_back(part);
        }
        
        // Check if we have at least provider and model
        if (parts.size() >= 2) {
            std::string listedProvider = parts[0];
            std::string listedModel = parts[1];
            
            // If provider and model match, it's blacklisted
            if (listedProvider == provider && listedModel == modelName) {
                return true;
            }
        }
    }
    
    return false;
}

/**
 * Adds a model to the blacklist for a specific provider.
 * @param provider The provider name.
 * @param modelName The name of the model to blacklist.
 * @param reason Optional reason why the model was blacklisted.
 */
void addModelToBlacklist(const std::string &provider, const std::string &modelName, const std::string &reason) {
    if (isModelBlacklisted(provider, modelName)) {
        // Model already blacklisted for this provider, no need to add it again
        std::cout << "Model '" << modelName << "' already blacklisted for provider '" << provider << "'." << std::endl;
        return;
    }
    
    std::string home = SystemUtils::getEnvVar("HOME");
    std::string configDir = home + "/.config/ai";
    std::string blacklistPath = configDir + "/blacklist";
    
    // Create the config directory if it doesn't exist
    if (!std::filesystem::exists(configDir)) {
        std::filesystem::create_directories(configDir);
    }
    
    // Open the blacklist file in append mode
    std::ofstream blacklistFile(blacklistPath, std::ios::app);
    if (!blacklistFile.is_open()) {
        std::cerr << "Error: Could not open blacklist file for writing" << std::endl;
        return;
    }
    
    // Add a timestamp to the entry
    std::time_t now = std::time(nullptr);
    std::string timestamp = std::ctime(&now);
    // Remove the newline character from ctime output
    timestamp.erase(timestamp.find_last_of('\n'));
    
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
 * @param provider The provider name.
 * @param modelName The name of the model to remove from the blacklist.
 */
void removeModelFromBlacklist(const std::string &provider, const std::string &modelName) {
    std::string home = SystemUtils::getEnvVar("HOME");
    std::string blacklistPath = home + "/.config/ai/blacklist";
    
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
            // Remove any comment portion for parsing
            std::string parseLine = line;
            size_t commentPos = parseLine.find('#');
            if (commentPos != std::string::npos) {
                parseLine = parseLine.substr(0, commentPos);
            }
            
            // Parse the line with pipe-separated values
            std::vector<std::string> parts;
            std::stringstream ss(parseLine);
            std::string part;
            
            while (std::getline(ss, part, '|')) {
                // Trim whitespace
                part.erase(0, part.find_first_not_of(" \t"));
                part.erase(part.find_last_not_of(" \t") + 1);
                parts.push_back(part);
            }
            
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
 * @return A vector of BlacklistEntry structures.
 */
std::vector<BlacklistEntry> getBlacklistedModels() {
    std::vector<BlacklistEntry> blacklistedModels;
    
    std::string home = SystemUtils::getEnvVar("HOME");
    std::string blacklistPath = home + "/.config/ai/blacklist";
    
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
        std::string comment;
        std::string timestamp;
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            std::string commentPart = line.substr(commentPos + 1);
            // Try to extract timestamp
            size_t addedOnPos = commentPart.find("Added on ");
            if (addedOnPos != std::string::npos) {
                timestamp = commentPart.substr(addedOnPos + 9); // "Added on " is 9 chars
                // Trim whitespace
                timestamp.erase(0, timestamp.find_first_not_of(" \t"));
                timestamp.erase(timestamp.find_last_not_of(" \t") + 1);
            }
            // Remove comment part from line for parsing
            line = line.substr(0, commentPos);
        }
        
        // Parse the line with pipe-separated values
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string part;
        
        while (std::getline(ss, part, '|')) {
            // Trim whitespace
            part.erase(0, part.find_first_not_of(" \t"));
            part.erase(part.find_last_not_of(" \t") + 1);
            parts.push_back(part);
        }
        
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