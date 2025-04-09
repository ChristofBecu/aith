#ifndef API_H
#define API_H

#include <string>

void listModels(const std::string &apiKey);
void chat(const std::string &prompt, const std::string &model, const std::string &apiKey, const std::string &currentHistory, bool newChat);

#endif // API_H