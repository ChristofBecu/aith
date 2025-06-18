#include "history.h"
#include "file_operations.h"
#include "json_file_handler.h"
#include "filename_generator.h"
#include "application_setup.h"
#include <json/json.h>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <iostream>

/**
 * Extracts the first user prompt from an existing history file.
 * This ensures that archived history files are named based on their actual content.
 * @param historyPath The path to the history file to read.
 * @return The first user prompt found, or empty string if none exists.
 */
std::string extractFirstUserPrompt(const std::string &historyPath) {
    try {
        Json::Value history = JsonFileHandler::read(historyPath);
        
        // Look for the first user message in the history
        if (history.isArray()) {
            for (const auto &message : history) {
                if (message.isMember("role") && message.isMember("content") && 
                    message["role"].asString() == "user") {
                    return message["content"].asString();
                }
            }
        }
    } catch (const std::exception&) {
        // If reading fails, return empty string (handled by caller)
    }
    
    return "";
}

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
        
        // CRITICAL FIX: Use the first prompt from the existing conversation being archived,
        // not the new prompt that's starting the next conversation
        std::string firstPrompt = extractFirstUserPrompt(currentHistory);
        if (firstPrompt.empty()) {
            // Fallback: if we can't extract the first prompt, use a generic name
            firstPrompt = "conversation";
        }
        
        // Generate filename based on the conversation being saved, not the new one starting
        std::string descriptiveName = FilenameGenerator::generateFromPrompt(firstPrompt, 45);
        
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
 * Starts a new history file and updates the current conversation name.
 * This is the enhanced version that supports dynamic conversation naming.
 * @param prompt The initial user prompt for the new conversation.
 * @param historyDir The directory where history files are stored.
 * @param currentHistory The path to the current history file.
 * @return The path to the new current history file.
 */
std::string startNewHistoryAndGetPath(const std::string &prompt, const std::string &historyDir, const std::string &currentHistory) {
    // Archive existing conversation if it exists
    if (FileOperations::exists(currentHistory)) {
        std::time_t now = std::time(nullptr);
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&now));
        
        // Use the first prompt from the existing conversation being archived
        std::string firstPrompt = extractFirstUserPrompt(currentHistory);
        if (firstPrompt.empty()) {
            firstPrompt = "conversation";
        }
        
        std::string descriptiveName = FilenameGenerator::generateFromPrompt(firstPrompt, 45);
        std::string baseFilename = "history_" + descriptiveName + "_" + timestamp;
        std::string uniqueFilename = FilenameGenerator::ensureUniqueFilename(historyDir, baseFilename, ".json");
        
        FileOperations::rename(currentHistory, historyDir + "/" + uniqueFilename + ".json");
    }
    
    // Generate descriptive name for the NEW conversation
    std::string newConversationName = FilenameGenerator::generateFromPrompt(prompt, 45);
    
    // Update the current conversation name in persistent storage
    ApplicationSetup::setCurrentConversationName(newConversationName);
    
    // Calculate the new current history path
    std::string newCurrentHistory = historyDir + "/current_" + newConversationName + ".json";
    
    // Create initial empty history array in the new file
    FileOperations::write(newCurrentHistory, "[]");
    
    // Add the initial user prompt to the new history
    Json::Value history(Json::arrayValue);
    Json::Value entry;
    entry["role"] = "user";
    entry["content"] = prompt;
    history.append(entry);
    
    JsonFileHandler::write(newCurrentHistory, history);
    
    return newCurrentHistory;
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