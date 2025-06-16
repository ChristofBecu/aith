#ifndef JSON_FILE_HANDLER_H
#define JSON_FILE_HANDLER_H

#include <string>
#include <json/json.h>

/**
 * @brief JSON file operations utility class
 * 
 * This class provides JSON-specific file operations including:
 * - Reading and parsing JSON files
 * - Writing JSON data to files with proper formatting
 * 
 * Follows Single Responsibility Principle by focusing only on JSON file operations.
 */
class JsonFileHandler {
public:
    /**
     * Reads and parses a JSON file
     * @param filePath Path to the JSON file
     * @return Parsed JSON value
     * @throws std::runtime_error if file cannot be read or parsed
     */
    static Json::Value read(const std::string& filePath);
    
    /**
     * Writes a JSON value to a file
     * @param filePath Path to the JSON file
     * @param json JSON value to write
     * @throws std::runtime_error if file cannot be written
     */
    static void write(const std::string& filePath, const Json::Value& json);
};

#endif // JSON_FILE_HANDLER_H
