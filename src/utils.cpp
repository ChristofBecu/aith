#include "utils.h"
#include "system_utils.h"
#include "config_manager.h"
#include "provider_manager.h"
#include "model_blacklist.h"
#include <cstdlib>
#include <stdexcept>
#include <array>
#include <memory>
#include <functional>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <vector>

// Backward compatibility wrappers (deprecated)
std::string getEnvVar(const std::string &key) {
    return SystemUtils::getEnvVar(key);
}

std::string exec(const char* cmd) {
    return SystemUtils::exec(cmd);
}

void executeCommand(const std::string &command) {
    SystemUtils::executeCommand(command);
}

std::string getConfigValue(const std::string &key) {
    return ConfigManager::getConfigValue(key);
}

std::string getProviderConfigValue(const std::string &provider, const std::string &key) {
    return ConfigManager::getProviderConfigValue(provider, key);
}

void setCommandLineProvider(const std::string &provider) {
    ProviderManager::setCommandLineProvider(provider);
}

std::string getDefaultProvider() {
    return ProviderManager::getDefaultProvider();
}

std::string getDefaultModel() {
    return ProviderManager::getDefaultModel();
}

std::string getApiUrl() {
    return ProviderManager::getApiUrl();
}

std::string getAgent() {
    return ProviderManager::getAgent();
}

std::string getApiKey() {
    return ProviderManager::getApiKey();
}

bool isModelBlacklisted(const std::string &provider, const std::string &modelName) {
    return ModelBlacklist::isModelBlacklisted(provider, modelName);
}

void addModelToBlacklist(const std::string &provider, const std::string &modelName, const std::string &reason) {
    ModelBlacklist::addModelToBlacklist(provider, modelName, reason);
}

void removeModelFromBlacklist(const std::string &provider, const std::string &modelName) {
    ModelBlacklist::removeModelFromBlacklist(provider, modelName);
}

std::vector<BlacklistEntry> getBlacklistedModels() {
    return ModelBlacklist::getBlacklistedModels();
}