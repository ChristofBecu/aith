#ifndef API_MANAGER_H
#define API_MANAGER_H

#include <string>

/**
 * @brief API management service for AITH application.
 * 
 * This class manages high-level API operations, coordinating with
 * various managers (ProviderManager, ConfigManager, BlacklistManager)
 * to provide unified API functionality for models and chat operations.
 */
class ApiManager {
public:
    /**
     * Lists all available models from the configured provider.
     * @param apiKey The API key for authentication
     */
    static void listModels(const std::string &apiKey);
    
    /**
     * Sends a chat request to the specified model.
     * @param prompt The input string to send to the model
     * @param model The model to use for the chat
     * @param apiKey The API key for authentication
     * @param currentHistory The path to the history file
     * @param newChat Whether this is a new chat session
     */
    static void chat(const std::string &prompt, const std::string &model, 
                     const std::string &apiKey, const std::string &currentHistory, 
                     bool newChat);
};

#endif // API_MANAGER_H
