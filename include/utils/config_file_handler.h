#ifndef CONFIG_FILE_HANDLER_H
#define CONFIG_FILE_HANDLER_H

#include <string>

/**
 * @brief Configuration file operations utility class
 * 
 * This class provides configuration file operations including:
 * - Reading key-value pairs from config files
 * - Writing key-value pairs to config files
 * - Parsing configuration file formats
 * 
 * Follows Single Responsibility Principle by focusing only on configuration file operations.
 */
class ConfigFileHandler {
public:
    /**
     * Reads a specific value from a key-value config file
     * @param configPath Path to the config file
     * @param key Key to look for
     * @return Value associated with the key, or empty string if not found
     * @throws std::runtime_error if config file cannot be read
     */
    static std::string readValue(const std::string& configPath, const std::string& key);
    
    /**
     * Writes a key-value pair to a config file
     * @param configPath Path to the config file
     * @param key Key to write
     * @param value Value to write
     * @throws std::runtime_error if config file cannot be written
     */
    static void writeValue(const std::string& configPath, const std::string& key, 
                          const std::string& value);
};

#endif // CONFIG_FILE_HANDLER_H
