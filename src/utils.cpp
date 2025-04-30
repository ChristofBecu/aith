#include "utils.h"
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

/**
 * Retrieves the value of an environment variable.
 * @param key The name of the environment variable.
 * @return The value of the environment variable, or an empty string if not found.
 */
std::string getEnvVar(const std::string &key) {
    const char *val = std::getenv(key.c_str());
    if (val == nullptr) {
        return "";
    }
    return std::string(val);
}

/**
 * Executes a shell command and returns the output.
 * @param cmd The command to execute.
 * @return The output of the command as a string.
 */
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, std::function<void(FILE*)>> pipe(popen(cmd, "r"), [](FILE* f) { pclose(f); });
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

/**
 * Executes a shell command and checks its return code.
 * @param command The command to execute.
 * If the command fails, the program will terminate with an error message.
 */
void executeCommand(const std::string &command) {
    int ret_code = std::system(command.c_str());
    if (ret_code != 0) {
        std::cerr << "Command failed: " << command << std::endl;
        std::exit(1);
    }
}

/**
 * Gets a configuration value from config file or environment variable.
 * Checks for the value in the following order:
 * 1. Environment variable with the given key
 * 2. Config file ~/.config/ai/config
 * 3. Falls back to the provided default value
 * @param key The configuration key to look for
 * @param defaultValue The default value to return if not found
 * @return The configuration value
 */
std::string getConfigValue(const std::string &key) {
    // First check environment variable
    std::string valueFromEnv = getEnvVar(key);
    if (!valueFromEnv.empty()) {
        return valueFromEnv;
    }
    
    // Then check config file
    std::string home = getEnvVar("HOME");
    std::string configDir = home + "/.config/ai";
    std::string configPath = configDir + "/config";
    
    if (std::filesystem::exists(configPath)) {
        std::ifstream configFile(configPath);
        if (configFile.is_open()) {
            std::string line;
            std::string searchKey = key + "=";
            while (std::getline(configFile, line)) {
                if (line.substr(0, searchKey.length()) == searchKey) {
                    std::string value = line.substr(searchKey.length());
                    // Remove quotes if present
                    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                        value = value.substr(1, value.length() - 2);
                    }
                    return value;
                }
            }
            configFile.close();
        }
    }
    
    // Fall back to default value
    return "";
}

/**
 * Gets a configuration value from a provider-specific config file.
 * Reads from ~/.config/ai/{provider}.conf
 * Checks both lowercase and uppercase variants of the provider name
 * @param provider The provider name (e.g., "groq", "openrouter")
 * @param key The configuration key to look for
 * @return The configuration value or empty string if not found
 */
std::string getProviderConfigValue(const std::string &provider, const std::string &key) {
    // First check environment variable with provider prefix
    std::string envVarName = provider + "_" + key;
    std::string valueFromEnv = getEnvVar(envVarName);
    if (!valueFromEnv.empty()) {
        return valueFromEnv;
    }
    
    // Also check uppercase environment variable
    std::string upperProvider = provider;
    std::transform(upperProvider.begin(), upperProvider.end(), upperProvider.begin(), ::toupper);
    envVarName = upperProvider + "_" + key;
    valueFromEnv = getEnvVar(envVarName);
    if (!valueFromEnv.empty()) {
        return valueFromEnv;
    }
    
    // Then check provider-specific config file
    std::string home = getEnvVar("HOME");
    std::string configDir = home + "/.config/ai";
    
    // Create a list of possible config file names with different cases
    std::vector<std::string> possibleConfigPaths = {
        configDir + "/" + provider + ".conf",                  // original case (e.g. openrouter.conf)
        configDir + "/" + upperProvider + ".conf",             // all uppercase (e.g. OPENROUTER.conf)
        configDir + "/" + provider.substr(0, 1) + provider.substr(1) + ".conf"  // first letter uppercase (e.g. Openrouter.conf)
    };
    
    // Try each possible config file path
    for (const auto& configPath : possibleConfigPaths) {
        if (std::filesystem::exists(configPath)) {
            std::ifstream configFile(configPath);
            if (configFile.is_open()) {
                std::string line;
                std::string searchKey = key + "=";
                while (std::getline(configFile, line)) {
                    if (line.substr(0, searchKey.length()) == searchKey) {
                        std::string value = line.substr(searchKey.length());
                        // Remove quotes if present
                        if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                            value = value.substr(1, value.length() - 2);
                        }
                        return value;
                    }
                }
                configFile.close();
            }
        }
    }
    
    // Fall back to empty string
    return "";
}

/**
 * Gets the default model from the provider's config file.
 * @return The default model name.
 */
std::string getDefaultModel() {
    std::string provider = getAgent();
    std::string model = getProviderConfigValue(provider, "DEFAULT_MODEL");
    if (model.empty()) {
        // Fall back to main config file
        model = getConfigValue("DEFAULT_MODEL");
    }
    return model;
}

/**
 * Gets the API URL from the provider's config file.
 * @return The API URL.
 */
std::string getApiUrl() {
    std::string provider = getAgent();
    std::string url = getProviderConfigValue(provider, "API_URL");
    if (url.empty()) {
        // Fall back to main config file
        url = getConfigValue("API_URL");
    }
    return url;
}

// Global variable to store the provider specified via command line
static std::string commandLineProvider = "";

/**
 * Sets the provider from command line arguments.
 * @param provider The provider specified via command line.
 */
void setCommandLineProvider(const std::string &provider) {
    commandLineProvider = provider;
}

/**
 * Gets the agent/provider type to use.
 * Priority order:
 * 1. Command line specified provider
 * 2. AGENT environment variable
 * 3. Default provider from config file
 * @return The agent/provider type
 */
std::string getAgent() {
    // First check if provider was specified via command line
    if (!commandLineProvider.empty()) {
        return commandLineProvider;
    }
    
    // Then check environment variable
    std::string agent = getEnvVar("AGENT");
    if (!agent.empty()) {
        return agent;
    }
    
    // Then check the config file
    agent = getConfigValue("AGENT");
    if (!agent.empty()) {
        return agent;
    }
    
    // Finally, fall back to the default provider
    return getDefaultProvider();
}

/**
 * Gets the API key from the provider's config file.
 * Checks for the API key in the following order:
 * 1. Environment variable with provider prefix (e.g., GROQ_API_KEY)
 * 2. Provider-specific config file (~/.config/ai/provider.conf)
 * 3. Returns empty string if not found
 * @return The API key.
 */
std::string getApiKey() {
    std::string provider = getAgent();
    
    // Try agent-specific environment variable
    std::string envVarName = provider + "_API_KEY";
    std::string apiKey = getEnvVar(envVarName);
    if (!apiKey.empty()) {
        return apiKey;
    }
    
    // For backward compatibility, try GROQ_API_KEY
    if (provider == "GROQ" || provider == "groq") {
        apiKey = getEnvVar("GROQ_API_KEY");
        if (!apiKey.empty()) {
            return apiKey;
        }
    }
    
    // Read from provider-specific config file
    apiKey = getProviderConfigValue(provider, "API_KEY");
    if (!apiKey.empty()) {
        return apiKey;
    }
    
    // Try generic API_KEY from main config file as last resort
    return getConfigValue("API_KEY");
}

/**
 * Gets the default provider from the main config file.
 * @return The default provider name (defaults to "groq" if not specified)
 */
std::string getDefaultProvider() {
    std::string provider = getConfigValue("DEFAULT_PROVIDER");
    return provider.empty() ? "groq" : provider;
}