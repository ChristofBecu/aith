#include "history.h"
#include "file_utils.h"
#include <json/json.h>
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
    if (FileUtils::fileExists(currentHistory)) {
        std::time_t now = std::time(nullptr);
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&now));
        std::string title = prompt;
        std::replace(title.begin(), title.end(), ' ', '_');
        title.erase(std::remove_if(title.begin(), title.end(), [](char c) { return !std::isalnum(c) && c != '_'; }), title.end());
        FileUtils::renameFile(currentHistory, historyDir + "/history_" + title + "_" + timestamp + ".json");
    }

    // Create initial empty history array
    FileUtils::writeFile(currentHistory, "[]");

    // Add the initial user prompt to the history
    Json::Value history(Json::arrayValue);
    Json::Value entry;
    entry["role"] = "user";
    entry["content"] = prompt;
    history.append(entry);

    FileUtils::writeJsonFile(currentHistory, history);
}

/**
 * Adds a new entry to the history file.
 * @param role The role of the entity (e.g., user, assistant) adding the entry.
 * @param content The content of the entry to add.
 * @param currentHistory The path to the current history file.
 */
void addToHistory(const std::string &role, const std::string &content, const std::string &currentHistory) {
    Json::Value history = FileUtils::readJsonFile(currentHistory);

    Json::Value entry;
    entry["role"] = role;
    entry["content"] = content;
    history.append(entry);

    FileUtils::writeJsonFile(currentHistory, history);
}

/**
 * Ensures that a history file exists, creating it with an empty JSON array if it doesn't.
 */
void ensureHistoryFileExists(const std::string &historyPath) {
    if (!FileUtils::fileExists(historyPath)) {
        FileUtils::writeFile(historyPath, "[]");
    }
}

/**
 * Loads chat history from a file.
 */
Json::Value loadChatHistory(const std::string &historyPath) {
    if (FileUtils::fileExists(historyPath)) {
        try {
            return FileUtils::readJsonFile(historyPath);
        } catch (const std::exception&) {
            // If reading fails, return empty array (maintains existing behavior)
            return Json::Value(Json::arrayValue);
        }
    }
    return Json::Value(Json::arrayValue);
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