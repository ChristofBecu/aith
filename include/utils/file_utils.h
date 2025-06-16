#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>
#include <json/json.h>

/**
 * File utility functions for temporary file management and file operations
 * Provides reusable file handling methods
 */
class FileUtils {
public:
    // ===== EXISTING METHODS =====
    
    /**
     * Creates a temporary file with the given content
     * @param content The content to write to the file
     * @param prefix The prefix for the temporary file name
     * @param suffix The suffix/extension for the temporary file name
     * @return Full path to the created temporary file
     */
    static std::string createTempFile(const std::string& content, 
                                      const std::string& prefix, 
                                      const std::string& suffix = ".tmp");
    
    /**
     * Creates a temporary file with JSON content
     * @param jsonContent The JSON content to write
     * @param prefix The prefix for the temporary file name
     * @return Full path to the created temporary JSON file
     */
    static std::string createTempJsonFile(const std::string& jsonContent, 
                                          const std::string& prefix);
    
    /**
     * Removes a file if it exists
     * @param filePath Path to the file to remove
     * @return True if file was removed or didn't exist, false on error
     */
    static bool removeFile(const std::string& filePath);
    
    /**
     * Removes multiple files
     * @param filePaths Vector of file paths to remove
     * @return Number of files successfully removed
     */
    static size_t removeFiles(const std::vector<std::string>& filePaths);
    
    /**
     * Checks if a file exists
     * @param filePath Path to the file to check
     * @return True if file exists, false otherwise
     */
    static bool fileExists(const std::string& filePath);
    
    /**
     * Gets the default temporary directory path
     * @return Path to temporary directory (typically /tmp on Unix systems)
     */
    static std::string getTempDirectory();

    // ===== NEW METHODS FOR PHASE 1 =====
    
    // Directory operations
    /**
     * Creates directories recursively (like mkdir -p)
     * @param dirPath Directory path to create
     * @throws std::runtime_error if directory creation fails
     */
    static void createDirectories(const std::string& dirPath);
    
    /**
     * Checks if a directory exists
     * @param dirPath Directory path to check
     * @return True if directory exists, false otherwise
     */
    static bool directoryExists(const std::string& dirPath);
    
    /**
     * Lists contents of a directory
     * @param dirPath Directory path to list
     * @param filenamesOnly If true, returns only filenames; if false, returns full paths
     * @return Vector of filenames or full paths
     */
    static std::vector<std::string> listDirectory(const std::string& dirPath, bool filenamesOnly = true);
    
    // File content operations
    /**
     * Reads all lines from a file
     * @param filePath Path to the file to read
     * @return Vector of lines from the file
     * @throws std::runtime_error if file cannot be read
     */
    static std::vector<std::string> readAllLines(const std::string& filePath);
    
    /**
     * Writes all lines to a file (overwrites existing content)
     * @param filePath Path to the file to write
     * @param lines Vector of lines to write
     * @throws std::runtime_error if file cannot be written
     */
    static void writeAllLines(const std::string& filePath, const std::vector<std::string>& lines);
    
    /**
     * Appends a single line to a file
     * @param filePath Path to the file to append to
     * @param line Line to append
     * @throws std::runtime_error if file cannot be written
     */
    static void appendLine(const std::string& filePath, const std::string& line);
    
    /**
     * Reads entire file content as a string
     * @param filePath Path to the file to read
     * @return File content as string
     * @throws std::runtime_error if file cannot be read
     */
    static std::string readFile(const std::string& filePath);
    
    /**
     * Writes content to a file (overwrites existing content)
     * @param filePath Path to the file to write
     * @param content Content to write
     * @throws std::runtime_error if file cannot be written
     */
    static void writeFile(const std::string& filePath, const std::string& content);
    
    // Config file operations
    /**
     * Reads a specific value from a key-value config file
     * @param configPath Path to the config file
     * @param key Key to look for
     * @return Value associated with the key, or empty string if not found
     * @throws std::runtime_error if config file cannot be read
     */
    static std::string readConfigValue(const std::string& configPath, const std::string& key);
    
    /**
     * Writes a key-value pair to a config file
     * @param configPath Path to the config file
     * @param key Key to write
     * @param value Value to write
     * @throws std::runtime_error if config file cannot be written
     */
    static void writeConfigValue(const std::string& configPath, const std::string& key, const std::string& value);
    
    // JSON file operations
    /**
     * Reads and parses a JSON file
     * @param filePath Path to the JSON file
     * @return Parsed JSON value
     * @throws std::runtime_error if file cannot be read or parsed
     */
    static Json::Value readJsonFile(const std::string& filePath);
    
    /**
     * Writes a JSON value to a file
     * @param filePath Path to the JSON file
     * @param json JSON value to write
     * @throws std::runtime_error if file cannot be written
     */
    static void writeJsonFile(const std::string& filePath, const Json::Value& json);
    
    // File manipulation
    /**
     * Renames/moves a file
     * @param oldPath Current file path
     * @param newPath New file path
     * @return True if successful, false otherwise
     */
    static bool renameFile(const std::string& oldPath, const std::string& newPath);
    
    /**
     * Copies a file
     * @param sourcePath Source file path
     * @param destPath Destination file path
     * @return True if successful, false otherwise
     */
    static bool copyFile(const std::string& sourcePath, const std::string& destPath);
    
    // File permissions and status
    /**
     * Checks if a file can be read
     * @param filePath Path to the file
     * @return True if file can be read, false otherwise
     */
    static bool canRead(const std::string& filePath);
    
    /**
     * Checks if a file can be written to
     * @param filePath Path to the file
     * @return True if file can be written, false otherwise
     */
    static bool canWrite(const std::string& filePath);
    
    /**
     * Gets the size of a file
     * @param filePath Path to the file
     * @return File size in bytes
     * @throws std::runtime_error if file doesn't exist or cannot be accessed
     */
    static std::size_t getFileSize(const std::string& filePath);
    
    /**
     * Gets the file extension
     * @param filePath Path to the file
     * @return File extension (including the dot), or empty string if no extension
     */
    static std::string getFileExtension(const std::string& filePath);
    
    // Enhanced temporary file operations
    /**
     * Creates a temporary file with timestamp in the name
     * @param content Content to write to the file
     * @param prefix Prefix for the temporary file name
     * @return Full path to the created temporary file
     */
    static std::string createTempFileWithTimestamp(const std::string& content, const std::string& prefix);
    
    /**
     * Creates a temporary directory path (but doesn't create the directory)
     * @return Path to a unique temporary directory
     */
    static std::string createTempDirectoryPath();
};

#endif // FILE_UTILS_H
