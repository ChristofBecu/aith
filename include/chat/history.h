#ifndef HISTORY_H
#define HISTORY_H

#include <string>
#include <json/json.h>

void startNewHistory(const std::string &prompt, const std::string &historyDir, const std::string &currentHistory);
std::string startNewHistoryAndGetPath(const std::string &prompt, const std::string &historyDir, const std::string &currentHistory);
void addToHistory(const std::string &role, const std::string &content, const std::string &currentHistory);

/**
 * Ensures that a history file exists, creating it with an empty JSON array if it doesn't.
 * @param historyPath The path to the history file to ensure exists.
 */
void ensureHistoryFileExists(const std::string &historyPath);

/**
 * Loads chat history from a file.
 * @param historyPath The path to the history file to load.
 * @return Json::Value containing the chat history array.
 */
Json::Value loadChatHistory(const std::string &historyPath);

/**
 * Builds the complete chat history by prepending a system message if provided.
 * @param history The existing chat history.
 * @param systemPrompt The system prompt to prepend (empty string to skip).
 * @return Json::Value containing the complete chat history with system message if provided.
 */
Json::Value buildChatHistoryWithSystem(const Json::Value &history, const std::string &systemPrompt);

#endif // HISTORY_H