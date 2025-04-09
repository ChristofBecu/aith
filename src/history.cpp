#include "history.h"
#include <json/json.h>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <iostream>

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