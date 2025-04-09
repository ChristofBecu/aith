#ifndef HISTORY_H
#define HISTORY_H

#include <string>

void startNewHistory(const std::string &prompt, const std::string &historyDir, const std::string &currentHistory);
void addToHistory(const std::string &role, const std::string &content, const std::string &currentHistory);

#endif // HISTORY_H