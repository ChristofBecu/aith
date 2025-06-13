#pragma once

#include <string>
#include <vector>

/**
 * @brief Configuration management for AITH application.
 * 
 * This class provides static methods for reading configuration values from
 * environment variables and configuration files. It handles both main
 * configuration (~/.config/aith/config) and provider-specific configurations
 * (~/.config/aith/{provider}.conf).
 */
class ConfigManager {
public:
    /**
     * Gets a configuration value from environment variable or main config file.
     * Checks for the value in the following order:
     * 1. Environment variable with the given key
     * 2. Main config file ~/.config/aith/config
     * 3. Returns empty string if not found
     * @param key The configuration key to look for
     * @return The configuration value or empty string if not found
     */
    static std::string getConfigValue(const std::string &key);
    
    /**
     * Gets a configuration value from a provider-specific config file.
     * Checks for the value in the following order:
     * 1. Environment variable with provider prefix (e.g., GROQ_API_KEY)
     * 2. Environment variable with uppercase provider prefix (e.g., GROQ_API_KEY)
     * 3. Provider-specific config file ~/.config/aith/{provider}.conf
     * 4. Returns empty string if not found
     * @param provider The provider name (e.g., "groq", "openrouter")
     * @param key The configuration key to look for
     * @return The configuration value or empty string if not found
     */
    static std::string getProviderConfigValue(const std::string &provider, const std::string &key);

    /**
     * Gets the default prompt content from the defaultprompt file.
     * Reads from ~/.config/aith/defaultprompt if it exists.
     * @return The default prompt content or empty string if file doesn't exist
     */
    static std::string getDefaultPrompt();

private:
    /**
     * Gets the main configuration directory path (~/.config/aith)
     * @return The configuration directory path
     */
    static std::string getConfigDir();
    
    /**
     * Reads a configuration value from a specific file.
     * @param configPath The path to the configuration file
     * @param key The configuration key to look for
     * @return The configuration value or empty string if not found
     */
    static std::string readConfigFile(const std::string &configPath, const std::string &key);
    
    /**
     * Removes quotes from a configuration value if present.
     * @param value The value to process
     * @return The value with quotes removed if they were present
     */
    static std::string removeQuotes(const std::string &value);
    
    /**
     * Generates possible config file paths for a provider with different case variations.
     * @param provider The provider name
     * @return Vector of possible config file paths
     */
    static std::vector<std::string> getProviderConfigPaths(const std::string &provider);
};
