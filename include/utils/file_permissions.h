#ifndef FILE_PERMISSIONS_H
#define FILE_PERMISSIONS_H

#include <string>

/**
 * @brief File permissions and security utility class
 * 
 * This class provides file permission operations including:
 * - Checking read/write permissions
 * - File access validation
 * 
 * Follows Single Responsibility Principle by focusing only on file permissions and security.
 */
class FilePermissions {
public:
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
};

#endif // FILE_PERMISSIONS_H
