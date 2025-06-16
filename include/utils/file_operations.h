#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <string>
#include <vector>

/**
 * @brief Core file operations utility class
 * 
 * This class provides essential file operations including:
 * - File existence checking
 * - Reading and writing files
 * - File manipulation (rename, copy)
 * - File metadata (size, extension)
 * - File removal operations
 * 
 * Follows Single Responsibility Principle by focusing only on basic file operations.
 */
class FileOperations {
public:
    // File existence and basic operations
    /**
     * Checks if a file exists
     * @param filePath Path to the file to check
     * @return True if file exists, false otherwise
     */
    static bool exists(const std::string& filePath);
    
    /**
     * Removes a file if it exists
     * @param filePath Path to the file to remove
     * @return True if file was removed or didn't exist, false on error
     */
    static bool remove(const std::string& filePath);
    
    /**
     * Removes multiple files
     * @param filePaths Vector of file paths to remove
     * @return Number of files successfully removed
     */
    static size_t removeMultiple(const std::vector<std::string>& filePaths);
    
    // File content operations
    /**
     * Reads entire file content as a string
     * @param filePath Path to the file to read
     * @return File content as string
     * @throws std::runtime_error if file cannot be read
     */
    static std::string read(const std::string& filePath);
    
    /**
     * Writes content to a file (overwrites existing content)
     * @param filePath Path to the file to write
     * @param content Content to write
     * @throws std::runtime_error if file cannot be written
     */
    static void write(const std::string& filePath, const std::string& content);
    
    /**
     * Appends a single line to a file
     * @param filePath Path to the file to append to
     * @param line Line to append
     * @throws std::runtime_error if file cannot be written
     */
    static void appendLine(const std::string& filePath, const std::string& line);
    
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
    
    // File manipulation
    /**
     * Renames/moves a file
     * @param oldPath Current file path
     * @param newPath New file path
     * @return True if successful, false otherwise
     */
    static bool rename(const std::string& oldPath, const std::string& newPath);
    
    /**
     * Copies a file
     * @param sourcePath Source file path
     * @param destPath Destination file path
     * @return True if successful, false otherwise
     */
    static bool copy(const std::string& sourcePath, const std::string& destPath);
    
    // File metadata
    /**
     * Gets the size of a file
     * @param filePath Path to the file
     * @return File size in bytes
     * @throws std::runtime_error if file doesn't exist or cannot be accessed
     */
    static std::size_t getSize(const std::string& filePath);
    
    /**
     * Gets the file extension
     * @param filePath Path to the file
     * @return File extension (including the dot), or empty string if no extension
     */
    static std::string getExtension(const std::string& filePath);
};

#endif // FILE_OPERATIONS_H
