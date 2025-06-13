#pragma once

#include <string>
#include <vector>

/**
 * @brief Manages file system operations for the blacklist system.
 * 
 * This class handles all file I/O operations for the blacklist including:
 * - Reading and writing blacklist files
 * - Directory creation and validation
 * - File existence and permission checks
 * - Path resolution for blacklist storage
 */
class BlacklistFileManager {
public:
    /**
     * @brief Constructor that initializes the blacklist file path
     */
    BlacklistFileManager();
    
    /**
     * @brief Destructor
     */
    ~BlacklistFileManager() = default;
    
    // File operations
    
    /**
     * @brief Checks if the blacklist file exists
     * @return True if the blacklist file exists, false otherwise
     */
    bool exists() const;
    
    /**
     * @brief Reads all lines from the blacklist file
     * @return Vector of strings containing all lines from the file
     * @throws std::runtime_error if file cannot be read
     */
    std::vector<std::string> readAllLines() const;
    
    /**
     * @brief Writes all lines to the blacklist file (overwrites existing content)
     * @param lines Vector of strings to write to the file
     * @throws std::runtime_error if file cannot be written
     */
    void writeAllLines(const std::vector<std::string>& lines) const;
    
    /**
     * @brief Appends a single line to the blacklist file
     * @param line The line to append to the file
     * @throws std::runtime_error if file cannot be written
     */
    void appendLine(const std::string& line) const;
    
    // Directory operations
    
    /**
     * @brief Ensures the config directory exists, creating it if necessary
     * @throws std::runtime_error if directory cannot be created
     */
    void ensureConfigDirectoryExists() const;
    
    /**
     * @brief Gets the full path to the blacklist file
     * @return The absolute path to the blacklist file
     */
    std::string getBlacklistPath() const;
    
    // Validation operations
    
    /**
     * @brief Checks if the blacklist file can be read
     * @return True if the file can be read, false otherwise
     */
    bool canRead() const;
    
    /**
     * @brief Checks if the blacklist file can be written
     * @return True if the file can be written, false otherwise
     */
    bool canWrite() const;

private:
    std::string blacklistPath_;
    
    /**
     * @brief Resolves the blacklist file path using system utilities
     * @return The resolved blacklist file path
     */
    std::string resolveBlacklistPath() const;
    
    /**
     * @brief Gets the config directory path
     * @return The config directory path
     */
    std::string getConfigDirectory() const;
    
    // Non-copyable and non-movable
    BlacklistFileManager(const BlacklistFileManager&) = delete;
    BlacklistFileManager& operator=(const BlacklistFileManager&) = delete;
    BlacklistFileManager(BlacklistFileManager&&) = delete;
    BlacklistFileManager& operator=(BlacklistFileManager&&) = delete;
};
