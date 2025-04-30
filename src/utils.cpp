#include "utils.h"
#include <cstdlib>
#include <stdexcept>
#include <array>
#include <memory>
#include <functional>
#include <iostream>
#include <filesystem>
#include <fstream>

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
 * Gets the default model from config file or environment variable.
 * @return The default model name.
 */
std::string getDefaultModel() {
    return getConfigValue("DEFAULT_MODEL");
}

/**
 * Gets the API URL from config file or environment variable.
 * @return The API URL.
 */
std::string getApiUrl() {
    return getConfigValue("API_URL");
}

/**
 * Gets the agent type from config file or environment variable.
 * @return The agent type (defaults to "GROQ").
 */
std::string getAgent() {
    return getConfigValue("AGENT");
}

/**
 * Gets the API key from config file or environment variable.
 * Checks for the API key in the following order:
 * 1. Environment variable GROQ_API_KEY (legacy), OPENAI_API_KEY, etc.
 * 2. Config file ~/.config/ai/config with API_KEY entry
 * 3. Returns empty string if not found
 * @return The API key.
 */
std::string getApiKey() {
    std::string agent = getAgent();
    
    // Try agent-specific environment variable
    std::string envVarName = agent + "_API_KEY";
    std::string apiKey = getEnvVar(envVarName);
    if (!apiKey.empty()) {
        return apiKey;
    }
    
    // For backward compatibility, try GROQ_API_KEY
    if (agent == "GROQ") {
        apiKey = getEnvVar("GROQ_API_KEY");
        if (!apiKey.empty()) {
            return apiKey;
        }
    }
    
    // Try generic API_KEY from config file
    return getConfigValue("API_KEY");
}