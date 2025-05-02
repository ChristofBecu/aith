#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

std::string getEnvVar(const std::string &key);
std::string exec(const char* cmd);
void executeCommand(const std::string &command);
std::string getConfigValue(const std::string &key);
std::string getProviderConfigValue(const std::string &provider, const std::string &key);
void setCommandLineProvider(const std::string &provider);
std::string getDefaultProvider();
std::string getDefaultModel();
std::string getApiUrl();
std::string getAgent();
std::string getApiKey();

// Blacklist functions with provider support
bool isModelBlacklisted(const std::string &provider, const std::string &modelName);
void addModelToBlacklist(const std::string &provider, const std::string &modelName, const std::string &reason = "");
void removeModelFromBlacklist(const std::string &provider, const std::string &modelName);
struct BlacklistEntry {
    std::string provider;
    std::string model;
    std::string reason;
    std::string timestamp;
};
std::vector<BlacklistEntry> getBlacklistedModels();

#endif // UTILS_H