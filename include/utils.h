#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string getEnvVar(const std::string &key);
std::string exec(const char* cmd);
void executeCommand(const std::string &command);
std::string getConfigValue(const std::string &key);
std::string getDefaultModel();
std::string getApiUrl();
std::string getAgent();
std::string getApiKey();

#endif // UTILS_H