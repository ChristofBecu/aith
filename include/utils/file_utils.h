#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>

/**
 * File utility functions for temporary file management and file operations
 * Provides reusable file handling methods
 */
class FileUtils {
public:
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
};

#endif // FILE_UTILS_H
