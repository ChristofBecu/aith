#include "history.h"
#include <json/json.h>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <iostream>

/**
 * Starts a new history file for storing chat interactions.
 * @param prompt The initial user prompt to store in the history.
 * @param historyDir The directory where history files are stored.
 * @param currentHistory The path to the current history file.
 */
void startNewHistory(const std::string &prompt, const std::string &historyDir, const std::string &currentHistory) {
    if (std::filesystem::exists(currentHistory)) {
        std::time_t now = std::time(nullptr);
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&now));
        std::string title = prompt;
        std::replace(title.begin(), title.end(), ' ', '_');
        title.erase(std::remove_if(title.begin(), title.end(), [](char c) { return !std::isalnum(c) && c != '_'; }), title.end());
        std::filesystem::rename(currentHistory, historyDir + "/history_" + title + "_" + timestamp + ".json");
    }

    std::ofstream file(currentHistory);
    file << "[]";
    file.close();

    Json::Value history(Json::arrayValue);
    Json::Value entry;
    entry["role"] = "user";
    entry["content"] = prompt;
    history.append(entry);

    std::ofstream historyFile(currentHistory);
    historyFile << history;
    historyFile.close();
}

/**
 * Adds a new entry to the history file.
 * @param role The role of the entity (e.g., user, assistant) adding the entry.
 * @param content The content of the entry to add.
 * @param currentHistory The path to the current history file.
 */
void addToHistory(const std::string &role, const std::string &content, const std::string &currentHistory) {
    std::ifstream file(currentHistory);
    Json::Value history;
    file >> history;
    file.close();

    Json::Value entry;
    entry["role"] = role;
    entry["content"] = content;
    history.append(entry);

    std::ofstream historyFile(currentHistory);
    historyFile << history;
    historyFile.close();
}

/**
 * Ensures that a history file exists, creating it with an empty JSON array if it doesn't.
 */
void ensureHistoryFileExists(const std::string &historyPath) {
    if (!std::filesystem::exists(historyPath)) {
        std::ofstream file(historyPath);
        file << "[]";
        file.close();
    }
}

/**
 * Loads chat history from a file.
 */
Json::Value loadChatHistory(const std::string &historyPath) {
    std::ifstream file(historyPath);
    Json::Value history;
    if (file.is_open()) {
        file >> history;
        file.close();
    }
    return history;
}

/**
 * Builds the complete chat history by prepending a system message if provided.
 */
Json::Value buildChatHistoryWithSystem(const Json::Value &history, const std::string &systemPrompt) {
    if (systemPrompt.empty()) {
        return history;
    }
    
    Json::Value systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = systemPrompt;

    Json::Value newHistory(Json::arrayValue);
    newHistory.append(systemMessage);
    for (const auto &message : history) {
        newHistory.append(message);
    }
    
    return newHistory;
}