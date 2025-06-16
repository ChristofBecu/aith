#include "application_setup.h"
#include "provider_manager.h"
#include "system_utils.h"
#include "directory_operations.h"
#include <iostream>
#include <stdexcept>

/**
 * Initializes the application and returns a complete configuration.
 * This method extracts the initialization logic from main.cpp.
 */
ApplicationSetup::Config ApplicationSetup::initialize() {
    Config config;
    
    // Get current provider and API key
    // Note: Provider override should already be set via CommandLineParser
    config.provider = ProviderManager::getAgent();
    config.apiKey = ProviderManager::getApiKey();
    
    // Validate API key is available
    validateApiKey(config);
    
    // Setup directory structure
    config.historyDir = getHistoryDirectoryPath();
    config.currentHistory = getCurrentHistoryPath(config.historyDir);
    
    // Ensure directories exist
    ensureDirectoriesExist(config.historyDir);
    
    return config;
}

/**
 * Validates that the API key is available for the current provider.
 * Extracted from main.cpp lines 29-35.
 */
void ApplicationSetup::validateApiKey(const Config& config) {
    if (config.apiKey.empty()) {
        std::string errorMessage = "Error: No API key found for provider '" + config.provider + "'";
        std::string helpMessage = "Please set API_KEY in ~/.config/aith/" + config.provider + 
                                ".conf or use " + config.provider + "_API_KEY environment variable.";
        
        std::cerr << errorMessage << std::endl;
        std::cerr << helpMessage << std::endl;
        
        throw std::runtime_error("Missing API key for provider: " + config.provider);
    }
}

/**
 * Ensures that all required directories exist.
 * Extracted from main.cpp line 41.
 */
void ApplicationSetup::ensureDirectoriesExist(const std::string& historyDir) {
    try {
        DirectoryOperations::create(historyDir);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create history directory: " + std::string(e.what()));
    }
}

/**
 * Constructs the history directory path based on user's home directory.
 * Extracted from main.cpp lines 37-38.
 */
std::string ApplicationSetup::getHistoryDirectoryPath() {
    std::string home = SystemUtils::getEnvVar("HOME");
    if (home.empty()) {
        throw std::runtime_error("HOME environment variable not set");
    }
    return home + "/aith_histories";
}

/**
 * Constructs the current history file path.
 * Extracted from main.cpp line 39.
 */
std::string ApplicationSetup::getCurrentHistoryPath(const std::string& historyDir) {
    return historyDir + "/current_history.json";
}
