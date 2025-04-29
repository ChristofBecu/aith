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
 * Gets the default model from config file or environment variable.
 * Checks for the model name in the following order:
 * 1. Environment variable DEFAULT_MODEL
 * 2. Config file ~/.config/ai/config
 * 3. Falls back to a hardcoded default
 * @return The default model name.
 */
std::string getDefaultModel() {
    // First check environment variable
    std::string modelFromEnv = getEnvVar("DEFAULT_MODEL");
    if (!modelFromEnv.empty()) {
        return modelFromEnv;
    }
    
    // Then check config file
    std::string home = getEnvVar("HOME");
    std::string configDir = home + "/.config/ai";
    std::string configPath = configDir + "/config";
    
    if (std::filesystem::exists(configPath)) {
        std::ifstream configFile(configPath);
        if (configFile.is_open()) {
            std::string line;
            while (std::getline(configFile, line)) {
                if (line.substr(0, 13) == "DEFAULT_MODEL=") {
                    std::string modelName = line.substr(13);
                    // Remove quotes if present
                    if (modelName.front() == '"' && modelName.back() == '"') {
                        modelName = modelName.substr(1, modelName.length() - 2);
                    }
                    return modelName;
                }
            }
            configFile.close();
        }
    }
    
    // Fall back to hardcoded default
    return "llama3-70b-8192";
}