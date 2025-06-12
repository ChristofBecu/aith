#include "api.h"
#include "system_utils.h"
#include "provider_manager.h"
#include "model_blacklist.h"
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <filesystem>
#include "markdown.h"
#include "history.h"

/**
 * Lists all available AI models.
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
    
    std::string command = "curl -s -X GET " + apiUrl + "/models -H 'Authorization: Bearer " + apiKey + "'";
    std::string result = SystemUtils::exec(command.c_str());

    Json::Value data;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(result);
    if (!Json::parseFromStream(reader, s, &data, &errs)) {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        std::cerr << "Response: " << result << std::endl;
        return;
    }

    // Check if we have data array in the response (OpenAI compatible APIs)
    if (data.isMember("data") && data["data"].isArray()) {
        for (const auto &model : data["data"]) {
            if (model.isMember("id")) {
                std::string modelId = model["id"].asString();
                std::string info = modelId;
                
                // Check if model is blacklisted for this provider and mark it
                if (ModelBlacklist::isModelBlacklisted(provider, modelId)) {
                    info = "BLACKLISTED: " + info;
                }
                
                if (model.isMember("owned_by")) {
                    info += " | Owner: " + model["owned_by"].asString();
                }
                
                if (model.isMember("created") && model["created"].isInt64()) {
                    std::time_t created = model["created"].asInt64();
                    std::string date = std::asctime(std::localtime(&created));
                    info += " | Created: " + date;
                }
                
                std::cout << info << std::endl;
            }
        }
    } 
    // Some APIs might have different response format
    else if (data.isMember("models") && data["models"].isArray()) {
        for (const auto &model : data["models"]) {
            if (model.isMember("id")) {
                std::string modelId = model["id"].asString();
                std::string info = modelId;
                
                // Check if model is blacklisted for this provider and mark it
                if (ModelBlacklist::isModelBlacklisted(provider, modelId)) {
                    info = "BLACKLISTED: " + info;
                }
                
                std::cout << info << std::endl;
            }
        }
    }
    // If we can't find any recognizable format, show raw response
    else {
        std::cerr << "Unexpected response format. Raw response:" << std::endl;
        std::cout << result << std::endl;
    }
}

/**
 * Sends a chat request to the AI model.
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
        std::cerr << "Use 'ai blacklist list' to see blacklisted models, or 'ai blacklist remove " << provider << " " << selectedModel << "' to unblacklist it." << std::endl;
        return;
    }

    std::string home = SystemUtils::getEnvVar("HOME");
    std::string defaultPromptPath = home + "/.config/ai/defaultprompt";
    std::string defaultPrompt;
    
    // Try to load default prompt from file
    if (std::filesystem::exists(defaultPromptPath)) {
        std::ifstream defaultPromptFile(defaultPromptPath);
        std::ostringstream buffer;
        buffer << defaultPromptFile.rdbuf();
        defaultPrompt = buffer.str();
        defaultPromptFile.close();
    }

    // Create history file if it doesn't exist
    if (!std::filesystem::exists(currentHistory)) {
        std::ofstream file(currentHistory);
        file << "[]";
        file.close();
    }

    // Add user message to history if not a new chat
    if (!newChat) {
        addToHistory("user", prompt, currentHistory);
    }

    // Load chat history
    std::ifstream file(currentHistory);
    Json::Value history;
    file >> history;
    file.close();

    // Add system message if default prompt exists
    if (!defaultPrompt.empty()) {
        Json::Value systemMessage;
        systemMessage["role"] = "system";
        systemMessage["content"] = defaultPrompt;

        Json::Value newHistory(Json::arrayValue);
        newHistory.append(systemMessage);
        for (const auto &message : history) {
            newHistory.append(message);
        }
        history = newHistory;
    }

    // Build the request payload
    Json::Value payload;
    payload["model"] = selectedModel;
    payload["messages"] = history;

    // Convert payload to JSON string
    Json::StreamWriterBuilder writer;
    std::string payloadJson = Json::writeString(writer, payload);

    // Save payload to temp file
    std::string tempFile = "/tmp/payload.json";
    std::ofstream outFile(tempFile);
    outFile << payloadJson;
    outFile.close();

    std::cout << "Sending request to " << provider << " using model " << selectedModel << "..." << std::endl;
    
    // Send request to API
    std::string command = "curl -s -X POST " + apiUrl + "/chat/completions -H 'Authorization: Bearer " + apiKey + "' -H 'Content-Type: application/json' -d @" + tempFile;
    std::string response = SystemUtils::exec(command.c_str());

    // Parse response
    Json::Value data;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(response);
    if (!Json::parseFromStream(reader, s, &data, &errs)) {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        std::cerr << "Response: " << response << std::endl;
        return;
    }

    // Check if response has expected format
    if (!data.isMember("choices") || !data["choices"][0].isMember("message")) {
        std::cerr << "Invalid API response received:" << std::endl;
        std::cerr << response << std::endl;
        return;
    }

    // Extract and display answer
    std::string answer = data["choices"][0]["message"]["content"].asString();
    renderMarkdown(answer);

    // Add response to history
    addToHistory("assistant", answer, currentHistory);
}