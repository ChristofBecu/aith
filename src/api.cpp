#include "api.h"
#include "system_utils.h"
#include "provider_manager.h"
#include "model_blacklist.h"
#include "config_manager.h"
#include "http_client.h"
#include "api_models.h"
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <filesystem>
#include "markdown.h"
#include "history.h"

/**
 * Lists all available aith models.
 * @param apiKey The API key for authentication.
 */
void listModels(const std::string &apiKey) {
    std::string apiUrl = ProviderManager::getApiUrl();
    std::string provider = ProviderManager::getAgent();
    
    if (apiUrl.empty()) {
        std::cerr << "Error: No API URL configured for provider '" << provider << "'" << std::endl;
        std::cerr << "Please check your " << provider << ".conf file or set the API_URL environment variable." << std::endl;
        return;
    }
    
    std::cout << "Fetching models from " << provider << " API..." << std::endl;
    
    std::string result = HttpClient::get(apiUrl + "/models", apiKey);
    
    ModelsListResponse response(result, provider);
    
    if (response.hasError()) {
        std::cerr << "Error: " << response.getErrorMessage() << std::endl;
        std::cerr << "Response: " << result << std::endl;
        return;
    }
    
    response.printModels();
}

/**
 * Sends a chat request to the aith model.
 * @param prompt The input string to send to the model.
 * @param model The model to use for the chat.
 * @param apiKey The API key for authentication.
 * @param currentHistory The path to the history file.
 * @param newChat Whether this is a new chat session.
 */
void chat(const std::string &prompt, const std::string &model, const std::string &apiKey, const std::string &currentHistory, bool newChat) {
    std::string selectedModel = model.empty() ? ProviderManager::getDefaultModel() : model;
    std::string apiUrl = ProviderManager::getApiUrl();
    std::string provider = ProviderManager::getAgent();
    
    std::cout << "Using provider: " << provider << std::endl;
    std::cout << "Using model: " << selectedModel << std::endl;
    std::cout << "Using API URL: " << apiUrl << std::endl;
    

    // Check if the API URL and model are available
    if (apiUrl.empty()) {
        std::cerr << "Error: No API URL configured for provider " << provider << std::endl;
        std::cerr << "Please check your " << provider << ".conf file or set the API_URL environment variable." << std::endl;
        return;
    }
    
    if (selectedModel.empty()) {
        std::cerr << "Error: No model specified and no DEFAULT_MODEL configured for provider " << provider << std::endl;
        std::cerr << "Please specify a model or set the DEFAULT_MODEL in your " << provider << ".conf file." << std::endl;
        return;
    }
    
    // Check if the model is blacklisted for this provider
    if (ModelBlacklist::isModelBlacklisted(provider, selectedModel)) {
        std::cerr << "Error: The model '" << selectedModel << "' is blacklisted for provider '" << provider << "'." << std::endl;
        std::cerr << "Use 'aith blacklist list' to see blacklisted models, or 'aith blacklist remove " << provider << " " << selectedModel << "' to unblacklist it." << std::endl;
        return;
    }

    // Load default prompt using ConfigManager
    std::string defaultPrompt = ConfigManager::getDefaultPrompt();

    // Ensure history file exists
    ensureHistoryFileExists(currentHistory);

    // Add user message to history if not a new chat
    if (!newChat) {
        addToHistory("user", prompt, currentHistory);
    }

    // Load and build chat history with system prompt
    Json::Value history = loadChatHistory(currentHistory);
    history = buildChatHistoryWithSystem(history, defaultPrompt);

    // Create and send chat request
    ChatRequest request(selectedModel, history);
    
    std::cout << "Sending request to " << provider << " using model " << selectedModel << "..." << std::endl;
    
    std::string responseJson = HttpClient::post(apiUrl + "/chat/completions", apiKey, request.toJson());
    
    // Parse response
    ChatResponse response(responseJson);
    
    if (response.hasError()) {
        std::cerr << "Error: " << response.getErrorMessage() << std::endl;
        std::cerr << "Response: " << responseJson << std::endl;
        return;
    }

    // Display and save response
    renderMarkdown(response.getContent());
    addToHistory("assistant", response.getContent(), currentHistory);
}