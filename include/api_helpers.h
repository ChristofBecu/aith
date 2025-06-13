#pragma once

#include <string>

/**
 * @brief Helper class for API validation operations.
 * 
 * This class provides static methods for validating API configurations
 * and requirements before making API calls.
 */
class ApiValidator {
public:
    /**
     * Validates that the provider configuration is complete for model listing.
     * @param provider The provider name
     * @param apiUrl The API URL
     * @return True if valid, false otherwise (error messages are printed)
     */
    static bool validateProviderForModels(const std::string& provider, const std::string& apiUrl);
    
    /**
     * Validates that the provider and model configuration is complete for chat.
     * @param provider The provider name
     * @param apiUrl The API URL
     * @param selectedModel The selected model name
     * @return True if valid, false otherwise (error messages are printed)
     */
    static bool validateProviderForChat(const std::string& provider, const std::string& apiUrl, const std::string& selectedModel);
};

/**
 * @brief Helper class for API console output operations.
 * 
 * This class provides static methods for displaying API-related information
 * and status messages to the console.
 */
class ApiConsole {
public:
    /**
     * Displays the status information for a chat request.
     * @param provider The provider name
     * @param model The model name
     * @param apiUrl The API URL
     */
    static void displayChatStatus(const std::string& provider, const std::string& model, const std::string& apiUrl);
    
    /**
     * Displays a progress message for model fetching.
     * @param provider The provider name
     */
    static void displayModelsFetchStatus(const std::string& provider);
    
    /**
     * Displays a progress message for chat request.
     * @param provider The provider name
     * @param model The model name
     */
    static void displayChatRequestStatus(const std::string& provider, const std::string& model);
    
    /**
     * Displays an error message with optional response details.
     * @param errorMessage The main error message
     * @param response Optional raw response for debugging
     */
    static void displayError(const std::string& errorMessage, const std::string& response = "");
};
