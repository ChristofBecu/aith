#include "file_utils.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <chrono>
#include <json/json.h>

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

// ===== NEW METHODS FOR PHASE 1 =====

// Directory operations
void FileUtils::createDirectories(const std::string& dirPath) {
    try {
        std::filesystem::create_directories(dirPath);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create directories: " + dirPath + " - " + e.what());
    }
}

bool FileUtils::directoryExists(const std::string& dirPath) {
    return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
}

std::vector<std::string> FileUtils::listDirectory(const std::string& dirPath, bool filenamesOnly) {
    std::vector<std::string> entries;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (filenamesOnly) {
                entries.push_back(entry.path().filename().string());
            } else {
                entries.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to list directory: " + dirPath + " - " + e.what());
    }
    return entries;
}

// File content operations
std::vector<std::string> FileUtils::readAllLines(const std::string& filePath) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }
    
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

void FileUtils::writeAllLines(const std::string& filePath, const std::vector<std::string>& lines) {
    std::ofstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }
    
    for (const auto& line : lines) {
        file << line << std::endl;
    }
}

void FileUtils::appendLine(const std::string& filePath, const std::string& line) {
    std::ofstream file(filePath, std::ios::app);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for appending: " + filePath);
    }
    
    file << line << std::endl;
}

std::string FileUtils::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FileUtils::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }
    
    file << content;
}

// Config file operations
std::string FileUtils::readConfigValue(const std::string& configPath, const std::string& key) {
    std::ifstream file(configPath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + configPath);
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string fileKey = line.substr(0, pos);
            std::string fileValue = line.substr(pos + 1);
            
            // Trim whitespace
            fileKey.erase(0, fileKey.find_first_not_of(" \t"));
            fileKey.erase(fileKey.find_last_not_of(" \t") + 1);
            fileValue.erase(0, fileValue.find_first_not_of(" \t"));
            fileValue.erase(fileValue.find_last_not_of(" \t") + 1);
            
            if (fileKey == key) {
                return fileValue;
            }
        }
    }
    
    return ""; // Key not found
}

void FileUtils::writeConfigValue(const std::string& configPath, const std::string& key, const std::string& value) {
    std::vector<std::string> lines;
    bool keyFound = false;
    
    // Read existing file if it exists
    if (fileExists(configPath)) {
        lines = readAllLines(configPath);
    }
    
    // Update or add the key-value pair
    for (auto& line : lines) {
        if (!line.empty() && line[0] != '#') {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string fileKey = line.substr(0, pos);
                fileKey.erase(0, fileKey.find_first_not_of(" \t"));
                fileKey.erase(fileKey.find_last_not_of(" \t") + 1);
                
                if (fileKey == key) {
                    line = key + "=" + value;
                    keyFound = true;
                    break;
                }
            }
        }
    }
    
    // Add new key-value pair if not found
    if (!keyFound) {
        lines.push_back(key + "=" + value);
    }
    
    writeAllLines(configPath, lines);
}

// JSON file operations
Json::Value FileUtils::readJsonFile(const std::string& filePath) {
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open JSON file for reading: " + filePath);
    }
    
    Json::Value json;
    Json::CharReaderBuilder builder;
    std::string errors;
    
    if (!Json::parseFromStream(builder, file, &json, &errors)) {
        throw std::runtime_error("Failed to parse JSON file: " + filePath + " - " + errors);
    }
    
    return json;
}

void FileUtils::writeJsonFile(const std::string& filePath, const Json::Value& json) {
    std::ofstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open JSON file for writing: " + filePath);
    }
    
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(json, &file);
}

// File manipulation
bool FileUtils::renameFile(const std::string& oldPath, const std::string& newPath) {
    try {
        std::filesystem::rename(oldPath, newPath);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error renaming file from " << oldPath << " to " << newPath << ": " << e.what() << std::endl;
        return false;
    }
}

bool FileUtils::copyFile(const std::string& sourcePath, const std::string& destPath) {
    try {
        std::filesystem::copy_file(sourcePath, destPath, std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error copying file from " << sourcePath << " to " << destPath << ": " << e.what() << std::endl;
        return false;
    }
}

// File permissions and status
bool FileUtils::canRead(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

bool FileUtils::canWrite(const std::string& filePath) {
    if (fileExists(filePath)) {
        std::ofstream file(filePath, std::ios::app);
        return file.good();
    } else {
        // Try to create a file to test write permissions
        std::ofstream file(filePath);
        bool canWrite = file.good();
        file.close();
        if (canWrite) {
            removeFile(filePath); // Clean up test file
        }
        return canWrite;
    }
}

std::size_t FileUtils::getFileSize(const std::string& filePath) {
    try {
        return std::filesystem::file_size(filePath);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get file size: " + filePath + " - " + e.what());
    }
}

std::string FileUtils::getFileExtension(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.extension().string();
}

// Enhanced temporary file operations
std::string FileUtils::createTempFileWithTimestamp(const std::string& content, const std::string& prefix) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    
    std::string tempPath = getTempDirectory() + "/" + prefix + "_" + std::to_string(timestamp) + ".tmp";
    
    std::ofstream file(tempPath);
    if (!file) {
        throw std::runtime_error("Failed to create temporary file with timestamp: " + tempPath);
    }
    
    file << content;
    file.close();
    
    return tempPath;
}

std::string FileUtils::createTempDirectoryPath() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    
    return getTempDirectory() + "/aith_temp_" + std::to_string(timestamp);
}
