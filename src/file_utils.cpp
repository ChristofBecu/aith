#include "file_utils.h"
#include <fstream>
#include <filesystem>
#include <iostream>

/**
 * Creates a temporary file with the given content
 */
std::string FileUtils::createTempFile(const std::string& content, 
                                      const std::string& prefix, 
                                      const std::string& suffix) {
    std::string tempPath = getTempDirectory() + "/" + prefix + suffix;
    
    std::ofstream file(tempPath);
    if (!file) {
        throw std::runtime_error("Failed to create temporary file: " + tempPath);
    }
    
    file << content;
    file.close();
    
    return tempPath;
}

/**
 * Creates a temporary file with JSON content
 */
std::string FileUtils::createTempJsonFile(const std::string& jsonContent, 
                                          const std::string& prefix) {
    return createTempFile(jsonContent, prefix, ".json");
}

/**
 * Removes a file if it exists
 */
bool FileUtils::removeFile(const std::string& filePath) {
    try {
        if (std::filesystem::exists(filePath)) {
            return std::filesystem::remove(filePath);
        }
        return true; // File doesn't exist, consider it "successfully removed"
    } catch (const std::exception& e) {
        std::cerr << "Error removing file " << filePath << ": " << e.what() << std::endl;
        return false;
    }
}

/**
 * Removes multiple files
 */
size_t FileUtils::removeFiles(const std::vector<std::string>& filePaths) {
    size_t removedCount = 0;
    for (const auto& filePath : filePaths) {
        if (removeFile(filePath)) {
            removedCount++;
        }
    }
    return removedCount;
}

/**
 * Checks if a file exists
 */
bool FileUtils::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

/**
 * Gets the default temporary directory path
 */
std::string FileUtils::getTempDirectory() {
    return "/tmp"; // Standard Unix temporary directory
}
