#include "history.h"
#include "file_operations.h"
#include "json_file_handler.h"
#include "filename_generator.h"
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
    if (FileOperations::exists(currentHistory)) {
        std::time_t now = std::time(nullptr);
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&now));
        
        // Generate a concise, meaningful filename from the prompt
        std::string descriptiveName = FilenameGenerator::generateFromPrompt(prompt, 45);
        
        // Ensure the filename is unique in the history directory
        std::string baseFilename = "history_" + descriptiveName + "_" + timestamp;
        std::string uniqueFilename = FilenameGenerator::ensureUniqueFilename(historyDir, baseFilename, ".json");
        
        FileOperations::rename(currentHistory, historyDir + "/" + uniqueFilename + ".json");
    }

    // Create initial empty history array
    FileOperations::write(currentHistory, "[]");

    // Add the initial user prompt to the history
    Json::Value history(Json::arrayValue);
    Json::Value entry;
    entry["role"] = "user";
    entry["content"] = prompt;
    history.append(entry);

    JsonFileHandler::write(currentHistory, history);
}

/**
 * Adds a new entry to the history file.
 * @param role The role of the entity (e.g., user, assistant) adding the entry.
 * @param content The content of the entry to add.
 * @param currentHistory The path to the current history file.
 */
void addToHistory(const std::string &role, const std::string &content, const std::string &currentHistory) {
    Json::Value history = JsonFileHandler::read(currentHistory);

    Json::Value entry;
    entry["role"] = role;
    entry["content"] = content;
    history.append(entry);

    JsonFileHandler::write(currentHistory, history);
}

/**
 * Ensures that a history file exists, creating it with an empty JSON array if it doesn't.
 */
void ensureHistoryFileExists(const std::string &historyPath) {
    if (!FileOperations::exists(historyPath)) {
        FileOperations::write(historyPath, "[]");
    }
}

/**
 * Loads chat history from a file.
 */
Json::Value loadChatHistory(const std::string &historyPath) {
    if (FileOperations::exists(historyPath)) {
        try {
            return JsonFileHandler::read(historyPath);
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