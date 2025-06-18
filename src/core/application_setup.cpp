#include "application_setup.h"
#include "provider_manager.h"
#include "system_utils.h"
#include "directory_operations.h"
#include "file_operations.h"
#include "json_file_handler.h"
#include "filename_generator.h"
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
    config.currentConversationName = getCurrentConversationName();
    config.currentHistory = getCurrentHistoryPath(config.historyDir);
    
    // Ensure directories exist
    ensureDirectoriesExist(config.historyDir);
    
    // Handle backward compatibility migration
    migrateLegacyCurrentHistory(config);
    
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
    std::string conversationName = getCurrentConversationName();
    if (conversationName.empty()) {
        // Fallback to old naming for backward compatibility
        return historyDir + "/current_history.json";
    }
    return historyDir + "/current_" + conversationName + ".json";
}

/**
 * Gets the current conversation name from persistent storage.
 */
std::string ApplicationSetup::getCurrentConversationName() {
    std::string statePath = getConversationStatePath();
    
    if (!FileOperations::exists(statePath)) {
        return "";
    }
    
    try {
        std::string content = FileOperations::read(statePath);
        // Remove any trailing newlines or whitespace
        content.erase(content.find_last_not_of(" \n\r\t") + 1);
        return content;
    } catch (const std::exception&) {
        return "";
    }
}

/**
 * Sets the current conversation name in persistent storage.
 */
void ApplicationSetup::setCurrentConversationName(const std::string& conversationName) {
    std::string statePath = getConversationStatePath();
    
    try {
        FileOperations::write(statePath, conversationName);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not save conversation state: " << e.what() << std::endl;
    }
}

/**
 * Gets the path to the conversation state file.
 */
std::string ApplicationSetup::getConversationStatePath() {
    std::string home = SystemUtils::getEnvVar("HOME");
    if (home.empty()) {
        throw std::runtime_error("HOME environment variable not set");
    }
    return home + "/.config/aith/current_conversation";
}

/**
 * Migrates legacy current_history.json to the new dynamic naming system.
 */
void ApplicationSetup::migrateLegacyCurrentHistory(Config& config) {
    std::string legacyPath = config.historyDir + "/current_history.json";
    
    // If there's no legacy file and no current conversation name, nothing to do
    if (!FileOperations::exists(legacyPath) && config.currentConversationName.empty()) {
        return;
    }
    
    // If there's a legacy file but no current conversation name, migrate it
    if (FileOperations::exists(legacyPath) && config.currentConversationName.empty()) {
        try {
            // Read the legacy file to extract the first prompt
            Json::Value history = JsonFileHandler::read(legacyPath);
            std::string firstPrompt = "";
            
            if (history.isArray() && !history.empty()) {
                for (const auto &message : history) {
                    if (message.isMember("role") && message.isMember("content") && 
                        message["role"].asString() == "user") {
                        firstPrompt = message["content"].asString();
                        break;
                    }
                }
            }
            
            if (firstPrompt.empty()) {
                firstPrompt = "conversation";
            }
            
            // Generate a descriptive name and set it as current
            std::string conversationName = FilenameGenerator::generateFromPrompt(firstPrompt, 45);
            setCurrentConversationName(conversationName);
            
            // Update config
            config.currentConversationName = conversationName;
            config.currentHistory = getCurrentHistoryPath(config.historyDir);
            
            // Rename the legacy file to the new name
            FileOperations::rename(legacyPath, config.currentHistory);
            
            std::cout << "📁 Migrated legacy conversation to: current_" << conversationName << ".json" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not migrate legacy history file: " << e.what() << std::endl;
            // Leave legacy file as-is and create new conversation state
            setCurrentConversationName("conversation");
            config.currentConversationName = "conversation";
            config.currentHistory = getCurrentHistoryPath(config.historyDir);
        }
    }
}
