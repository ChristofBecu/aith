#include "config_manager.h"
#include "system_utils.h"
#include <filesystem>
#include <fstream>
#include <algorithm>

/**
 * Gets the main configuration directory path (~/.config/aith)
 */
std::string ConfigManager::getConfigDir() {
    std::string home = SystemUtils::getEnvVar("HOME");
    return home + "/.config/aith";
}

/**
 * Gets a configuration value from environment variable or main config file.
 * Checks for the value in the following order:
 * 1. Environment variable with the given key
 * 2. Main config file ~/.config/aith/config
 * 3. Returns empty string if not found
 */
std::string ConfigManager::getConfigValue(const std::string &key) {
    // First check environment variable
    std::string valueFromEnv = SystemUtils::getEnvVar(key);
    if (!valueFromEnv.empty()) {
        return valueFromEnv;
    }
    
    // Then check main config file
    std::string configDir = getConfigDir();
    std::string configPath = configDir + "/config";
    
    return readConfigFile(configPath, key);
}

/**
 * Gets a configuration value from a provider-specific config file.
 * Checks for the value in the following order:
 * 1. Environment variable with provider prefix (e.g., GROQ_API_KEY)
 * 2. Environment variable with uppercase provider prefix (e.g., GROQ_API_KEY)
 * 3. Provider-specific config file ~/.config/aith/{provider}.conf
 * 4. Returns empty string if not found
 */
std::string ConfigManager::getProviderConfigValue(const std::string &provider, const std::string &key) {
    // First check environment variable with provider prefix
    std::string envVarName = provider + "_" + key;
    std::string valueFromEnv = SystemUtils::getEnvVar(envVarName);
    if (!valueFromEnv.empty()) {
        return valueFromEnv;
    }
    
    // Also check uppercase environment variable
    std::string upperProvider = provider;
    std::transform(upperProvider.begin(), upperProvider.end(), upperProvider.begin(), ::toupper);
    envVarName = upperProvider + "_" + key;
    valueFromEnv = SystemUtils::getEnvVar(envVarName);
    if (!valueFromEnv.empty()) {
        return valueFromEnv;
    }
    
    // Then check provider-specific config files
    std::vector<std::string> possibleConfigPaths = getProviderConfigPaths(provider);
    
    // Try each possible config file path
    for (const auto& configPath : possibleConfigPaths) {
        if (std::filesystem::exists(configPath)) {
            std::string value = readConfigFile(configPath, key);
            if (!value.empty()) {
                return value;
            }
        }
    }
    
    // Fall back to empty string
    return "";
}

/**
 * Reads a configuration value from a specific file.
 */
std::string ConfigManager::readConfigFile(const std::string &configPath, const std::string &key) {
    if (!std::filesystem::exists(configPath)) {
        return "";
    }
    
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        return "";
    }
    
    std::string line;
    std::string searchKey = key + "=";
    
    while (std::getline(configFile, line)) {
        if (line.substr(0, searchKey.length()) == searchKey) {
            std::string value = line.substr(searchKey.length());
            return removeQuotes(value);
        }
    }
    
    return "";
}

/**
 * Removes quotes from a configuration value if present.
 */
std::string ConfigManager::removeQuotes(const std::string &value) {
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        return value.substr(1, value.length() - 2);
    }
    return value;
}

/**
 * Generates possible config file paths for a provider with different case variations.
 */
std::vector<std::string> ConfigManager::getProviderConfigPaths(const std::string &provider) {
    std::string configDir = getConfigDir();
    
    // Create uppercase version
    std::string upperProvider = provider;
    std::transform(upperProvider.begin(), upperProvider.end(), upperProvider.begin(), ::toupper);
    
    // Create first letter uppercase version
    std::string capitalizedProvider = provider;
    if (!capitalizedProvider.empty()) {
        capitalizedProvider[0] = std::toupper(capitalizedProvider[0]);
    }
    
    return {
        configDir + "/" + provider + ".conf",           // original case (e.g. openrouter.conf)
        configDir + "/" + upperProvider + ".conf",      // all uppercase (e.g. OPENROUTER.conf)
        configDir + "/" + capitalizedProvider + ".conf" // first letter uppercase (e.g. Openrouter.conf)
    };
}

/**
 * Gets the default prompt content from the defaultprompt file.
 * Reads from ~/.config/aith/defaultprompt if it exists.
 */
std::string ConfigManager::getDefaultPrompt() {
    std::string configDir = getConfigDir();
    std::string defaultPromptPath = configDir + "/defaultprompt";
    
    if (!std::filesystem::exists(defaultPromptPath)) {
        return "";
    }
    
    std::ifstream defaultPromptFile(defaultPromptPath);
    if (!defaultPromptFile.is_open()) {
        return "";
    }
    
    std::ostringstream buffer;
    buffer << defaultPromptFile.rdbuf();
    defaultPromptFile.close();
    
    return buffer.str();
}
