#ifndef DIRECTORY_OPERATIONS_H
#define DIRECTORY_OPERATIONS_H

#include <string>
#include <vector>

/**
 * @brief Directory operations utility class
 * 
 * This class provides directory-specific operations including:
 * - Directory creation and existence checking
 * - Directory listing and traversal
 * 
 * Follows Single Responsibility Principle by focusing only on directory operations.
 */
class DirectoryOperations {
public:
    /**
     * Creates directories recursively (like mkdir -p)
     * @param dirPath Directory path to create
     * @throws std::runtime_error if directory creation fails
     */
    static void create(const std::string& dirPath);
    
    /**
     * Checks if a directory exists
     * @param dirPath Directory path to check
     * @return True if directory exists, false otherwise
     */
    static bool exists(const std::string& dirPath);
    
    /**
     * Lists contents of a directory
     * @param dirPath Directory path to list
     * @param filenamesOnly If true, returns only filenames; if false, returns full paths
     * @return Vector of filenames or full paths
     * @throws std::runtime_error if directory cannot be read
     */
    static std::vector<std::string> list(const std::string& dirPath, bool filenamesOnly = true);
};

#endif // DIRECTORY_OPERATIONS_H
