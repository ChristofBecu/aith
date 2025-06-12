#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "system_utils.h"
#include "config_manager.h"
#include "provider_manager.h"
#include "model_blacklist.h"

// System utilities (backward compatibility - deprecated)
[[deprecated("Use SystemUtils::getEnvVar instead")]]
std::string getEnvVar(const std::string &key);
[[deprecated("Use SystemUtils::exec instead")]]
std::string exec(const char* cmd);
[[deprecated("Use SystemUtils::executeCommand instead")]]
void executeCommand(const std::string &command);

// Configuration utilities (backward compatibility - deprecated)
[[deprecated("Use ConfigManager::getConfigValue instead")]]
std::string getConfigValue(const std::string &key);
[[deprecated("Use ConfigManager::getProviderConfigValue instead")]]
std::string getProviderConfigValue(const std::string &provider, const std::string &key);

// Provider management utilities (backward compatibility - deprecated)
[[deprecated("Use ProviderManager::setCommandLineProvider instead")]]
void setCommandLineProvider(const std::string &provider);
[[deprecated("Use ProviderManager::getDefaultProvider instead")]]
std::string getDefaultProvider();
[[deprecated("Use ProviderManager::getDefaultModel instead")]]
std::string getDefaultModel();
[[deprecated("Use ProviderManager::getApiUrl instead")]]
std::string getApiUrl();
[[deprecated("Use ProviderManager::getAgent instead")]]
std::string getAgent();
[[deprecated("Use ProviderManager::getApiKey instead")]]
std::string getApiKey();

// Blacklist functions (backward compatibility - deprecated)
[[deprecated("Use ModelBlacklist::isModelBlacklisted instead")]]
bool isModelBlacklisted(const std::string &provider, const std::string &modelName);
[[deprecated("Use ModelBlacklist::addModelToBlacklist instead")]]
void addModelToBlacklist(const std::string &provider, const std::string &modelName, const std::string &reason = "");
[[deprecated("Use ModelBlacklist::removeModelFromBlacklist instead")]]
void removeModelFromBlacklist(const std::string &provider, const std::string &modelName);
[[deprecated("Use ModelBlacklist::getBlacklistedModels instead")]]
std::vector<BlacklistEntry> getBlacklistedModels();

#endif // UTILS_H