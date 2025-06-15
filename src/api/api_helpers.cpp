#include "api_helpers.h"
#include "blacklist_manager.h"
#include <iostream>

/**
 * Validates that the provider configuration is complete for model listing.
 */
bool ApiValidator::validateProviderForModels(const std::string& provider, const std::string& apiUrl) {
    if (apiUrl.empty()) {
        std::cerr << "Error: No API URL configured for provider '" << provider << "'" << std::endl;
        std::cerr << "Please check your " << provider << ".conf file or set the API_URL environment variable." << std::endl;
        return false;
    }
    return true;
}

/**
 * Validates that the provider and model configuration is complete for chat.
 */
bool ApiValidator::validateProviderForChat(const std::string& provider, const std::string& apiUrl, const std::string& selectedModel) {
    if (apiUrl.empty()) {
        std::cerr << "Error: No API URL configured for provider " << provider << std::endl;
        std::cerr << "Please check your " << provider << ".conf file or set the API_URL environment variable." << std::endl;
        return false;
    }
    
    if (selectedModel.empty()) {
        std::cerr << "Error: No model specified and no DEFAULT_MODEL configured for provider " << provider << std::endl;
        std::cerr << "Please specify a model or set the DEFAULT_MODEL in your " << provider << ".conf file." << std::endl;
        return false;
    }
    
    if (BlacklistManager::isModelBlacklisted(provider, selectedModel)) {
        std::cerr << "Error: The model '" << selectedModel << "' is blacklisted for provider '" << provider << "'." << std::endl;
        std::cerr << "Use 'aith blacklist list' to see blacklisted models, or 'aith blacklist remove " << provider << " " << selectedModel << "' to unblacklist it." << std::endl;
        return false;
    }
    
    return true;
}

/**
 * Displays the status information for a chat request.
 */
void ApiConsole::displayChatStatus(const std::string& provider, const std::string& model, const std::string& apiUrl) {
    std::cout << "Using provider: " << provider << std::endl;
    std::cout << "Using model: " << model << std::endl;
    std::cout << "Using API URL: " << apiUrl << std::endl;
}

/**
 * Displays a progress message for model fetching.
 */
void ApiConsole::displayModelsFetchStatus(const std::string& provider) {
    std::cout << "Fetching models from " << provider << " API..." << std::endl;
}

/**
 * Displays a progress message for chat request.
 */
void ApiConsole::displayChatRequestStatus(const std::string& provider, const std::string& model) {
    std::cout << "Sending request to " << provider << " using model " << model << "..." << std::endl;
}

/**
 * Displays an error message with optional response details.
 */
void ApiConsole::displayError(const std::string& errorMessage, const std::string& response) {
    std::cerr << "Error: " << errorMessage << std::endl;
    if (!response.empty()) {
        std::cerr << "Response: " << response << std::endl;
    }
}
