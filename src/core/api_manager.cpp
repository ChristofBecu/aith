#include "api_manager.h"
#include "system_utils.h"
#include "provider_manager.h"
#include "blacklist_manager.h"
#include "blacklist_manager.h"
#include "config_manager.h"
#include "http_client.h"
#include "api/api_models.h"
#include "api/api_helpers.h"
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
void ApiManager::listModels(const std::string &apiKey) {
    std::string apiUrl = ProviderManager::getApiUrl();
    std::string provider = ProviderManager::getAgent();
    
    if (!ApiValidator::validateProviderForModels(provider, apiUrl)) {
        return;
    }
    
    ApiConsole::displayModelsFetchStatus(provider);
    
    std::string result = HttpClient::get(apiUrl + "/models", apiKey);
    
    ModelsListResponse response(result, provider);
    
    if (response.hasError()) {
        ApiConsole::displayError(response.getErrorMessage(), result);
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
void ApiManager::chat(const std::string &prompt, const std::string &model, const std::string &apiKey, const std::string &currentHistory, bool newChat) {
    std::string selectedModel = model.empty() ? ProviderManager::getDefaultModel() : model;
    std::string apiUrl = ProviderManager::getApiUrl();
    std::string provider = ProviderManager::getAgent();
    
    ApiConsole::displayChatStatus(provider, selectedModel, apiUrl);

    if (!ApiValidator::validateProviderForChat(provider, apiUrl, selectedModel)) {
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
    
    ApiConsole::displayChatRequestStatus(provider, selectedModel);
    
    std::string responseJson = HttpClient::post(apiUrl + "/chat/completions", apiKey, request.toJson());
    
    // Parse response
    ChatResponse response(responseJson);
    
    if (response.hasError()) {
        ApiConsole::displayError(response.getErrorMessage(), responseJson);
        return;
    }

    // Display and save response
    std::string content = response.getContent();
    renderMarkdown(content);
    addToHistory("assistant", content, currentHistory);
}
