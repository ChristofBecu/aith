#include "temp_file_manager.h"
#include <fstream>
#include <chrono>
#include <stdexcept>

std::string TempFileManager::createTempFile(const std::string& content, 
                                            const std::string& prefix, 
                                            const std::string& suffix) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::string tempPath = getTempDirectory() + "/" + prefix + "_" + 
                          std::to_string(timestamp) + suffix;
    
    std::ofstream tempFile(tempPath);
    if (!tempFile.is_open()) {
        throw std::runtime_error("Failed to create temporary file: " + tempPath);
    }
    
    tempFile << content;
    tempFile.close();
    
    return tempPath;
}

std::string TempFileManager::createTempJsonFile(const std::string& jsonContent, 
                                                const std::string& prefix) {
    return createTempFile(jsonContent, prefix, ".json");
}

std::string TempFileManager::createTempFileWithTimestamp(const std::string& content, 
                                                         const std::string& prefix) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();
    
    std::string tempPath = getTempDirectory() + "/" + prefix + "_" + 
                          std::to_string(timestamp) + ".tmp";
    
    std::ofstream tempFile(tempPath);
    if (!tempFile.is_open()) {
        throw std::runtime_error("Failed to create temporary file: " + tempPath);
    }
    
    tempFile << content;
    tempFile.close();
    
    return tempPath;
}

std::string TempFileManager::getTempDirectory() {
    return "/tmp"; // Standard Unix temporary directory
}

std::string TempFileManager::createTempDirectoryPath() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    return getTempDirectory() + "/temp_dir_" + std::to_string(timestamp);
}
