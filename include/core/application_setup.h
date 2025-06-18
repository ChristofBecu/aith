#pragma once

#include <string>

/**
 * @brief Application setup and initialization for AITH application.
 * 
 * This class handles application-wide initialization tasks including:
 * - API key validation and error reporting
 * - Directory structure creation and history path setup
 * - Configuration validation
 * 
 * Provides a clean separation between application setup logic and
 * command execution, following the Single Responsibility Principle.
 */
class ApplicationSetup {
public:
    /**
     * @brief Configuration structure containing all application settings.
     * 
     * This structure holds the core configuration needed by the application
     * after initialization is complete.
     */
    struct Config {
        std::string apiKey;                ///< API key for the current provider
        std::string provider;              ///< Current AI provider name
        std::string historyDir;            ///< Directory path for history files
        std::string currentHistory;        ///< Path to current history file
        std::string currentConversationName; ///< Descriptive name for current conversation
    };

    /**
     * Initializes the application and returns a complete configuration.
     * 
     * This method performs all necessary setup operations:
     * - Retrieves and validates API key for current provider
     * - Creates necessary directory structure
     * - Sets up history file paths
     * - Validates all configuration is complete
     * 
     * Note: Provider override should be set via CommandLineParser before
     * calling this method, as it uses ProviderManager::setCommandLineProvider().
     * 
     * @return Complete application configuration
     * @throws std::runtime_error if initialization fails (missing API key, etc.)
     */
    static Config initialize();

    /**
     * Validates that the API key is available for the current provider.
     * 
     * Checks that the API key is properly configured and displays
     * appropriate error messages if not found, including guidance
     * on how to configure the API key.
     * 
     * @param config The configuration to validate
     * @throws std::runtime_error if API key is missing or invalid
     */
    static void validateApiKey(const Config& config);

    /**
     * Ensures that all required directories exist.
     * 
     * Creates the history directory and any other required directories
     * if they don't already exist. Uses filesystem API to create
     * directories recursively as needed.
     * 
     * @param historyDir The history directory path to create
     * @throws std::runtime_error if directory creation fails
     */
    static void ensureDirectoriesExist(const std::string& historyDir);
    
    /**
     * Gets the current conversation name from persistent storage.
     * 
     * @return The descriptive name of the current conversation, or empty string if none
     */
    static std::string getCurrentConversationName();
    
    /**
     * Sets the current conversation name in persistent storage and updates paths.
     * 
     * @param conversationName The descriptive name for the current conversation
     */
    static void setCurrentConversationName(const std::string& conversationName);
    
    /**
     * Constructs the current history file path.
     * 
     * @param historyDir The history directory path
     * @return Full path to the current history file (dynamic based on conversation name)
     */
    static std::string getCurrentHistoryPath(const std::string& historyDir);

private:
    /**
     * Constructs the history directory path based on user's home directory.
     * 
     * @return Full path to the aith_histories directory
     */
    static std::string getHistoryDirectoryPath();

    /**
     * Gets the path to the conversation state file.
     * 
     * @return Full path to the current conversation state file
     */
    static std::string getConversationStatePath();
    
    /**
     * Migrates legacy current_history.json to the new dynamic naming system.
     * 
     * @param config The application configuration to update
     */
    static void migrateLegacyCurrentHistory(Config& config);
};
