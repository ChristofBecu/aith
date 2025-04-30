#include "api.h"
#include "utils.h"
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
    std::string apiUrl = getApiUrl();
    std::string agent = getAgent();
    
    if (agent != "GROQ") {
        std::cerr << "Listing models for " << agent << " is not implemented yet." << std::endl;
        //return;
    }
    
    std::string command = "curl -s -X GET " + apiUrl + "/models -H 'Authorization: Bearer " + apiKey + "'";
    std::string result = exec(command.c_str());

    Json::Value data;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(result);
    if (!Json::parseFromStream(reader, s, &data, &errs)) {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        return;
    }

    for (const auto &model : data["data"]) {
        std::time_t created = model["created"].asInt64();
        std::string date = std::asctime(std::localtime(&created));
        std::cout << model["id"].asString() << " | Owner: " << model["owned_by"].asString() << " | Created: " << date;
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
    std::string selectedModel = model.empty() ? getDefaultModel() : model;
    std::string apiUrl = getApiUrl();
    std::string agent = getAgent();
    
    if (agent != "GROC") {
        std::cerr << "Chat with " << agent << " is not fully implemented yet." << std::endl;
        // Continue anyway with default implementation
    }

    std::string home = getEnvVar("HOME");
    std::string defaultPromptPath = home + "/.config/defaultprompt";
    std::string defaultPrompt;
    if (std::filesystem::exists(defaultPromptPath)) {
        std::ifstream defaultPromptFile(defaultPromptPath);
        std::ostringstream buffer;
        buffer << defaultPromptFile.rdbuf();
        defaultPrompt = buffer.str();
        defaultPromptFile.close();
    }

    if (!std::filesystem::exists(currentHistory)) {
        std::ofstream file(currentHistory);
        file << "[]";
        file.close();
    }

    if (!newChat) {
        addToHistory("user", prompt, currentHistory);
    }

    std::ifstream file(currentHistory);
    Json::Value history;
    file >> history;
    file.close();

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

    Json::Value payload;
    payload["model"] = selectedModel;
    payload["messages"] = history;

    Json::StreamWriterBuilder writer;
    std::string payloadJson = Json::writeString(writer, payload);

    std::string tempFile = "/tmp/payload.json";
    std::ofstream outFile(tempFile);
    outFile << payloadJson;
    outFile.close();

    std::string command = "curl -s -X POST " + apiUrl + "/chat/completions -H 'Authorization: Bearer " + apiKey + "' -H 'Content-Type: application/json' -d @" + tempFile;
    std::string response = exec(command.c_str());

    Json::Value data;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(response);
    if (!Json::parseFromStream(reader, s, &data, &errs)) {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        return;
    }

    if (!data.isMember("choices") || !data["choices"][0].isMember("message")) {
        std::cerr << "Invalid API response received:" << std::endl;
        std::cerr << response << std::endl;
        return;
    }

    std::string answer = data["choices"][0]["message"]["content"].asString();

    renderMarkdown(answer);

    addToHistory("assistant", answer, currentHistory);
}