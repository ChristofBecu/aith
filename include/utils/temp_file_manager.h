#ifndef TEMP_FILE_MANAGER_H
#define TEMP_FILE_MANAGER_H

#include <string>

/**
 * @brief Temporary file management utility class
 * 
 * This class provides temporary file operations including:
 * - Creating temporary files with various content types
 * - Managing temporary directory paths
 * - Timestamp-based temporary file naming
 * 
 * Follows Single Responsibility Principle by focusing only on temporary file management.
 */
class TempFileManager {
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
     * Creates a temporary file with timestamp in the name
     * @param content Content to write to the file
     * @param prefix Prefix for the temporary file name
     * @return Full path to the created temporary file
     */
    static std::string createTempFileWithTimestamp(const std::string& content, 
                                                   const std::string& prefix);
    
    /**
     * Gets the default temporary directory path
     * @return Path to temporary directory (typically /tmp on Unix systems)
     */
    static std::string getTempDirectory();
    
    /**
     * Creates a temporary directory path (but doesn't create the directory)
     * @return Path to a unique temporary directory
     */
    static std::string createTempDirectoryPath();
};

#endif // TEMP_FILE_MANAGER_H
