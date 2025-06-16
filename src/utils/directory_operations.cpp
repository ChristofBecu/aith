#include "directory_operations.h"
#include <filesystem>
#include <stdexcept>

void DirectoryOperations::create(const std::string& dirPath) {
    try {
        std::filesystem::create_directories(dirPath);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create directories: " + dirPath + " - " + e.what());
    }
}

bool DirectoryOperations::exists(const std::string& dirPath) {
    return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
}

std::vector<std::string> DirectoryOperations::list(const std::string& dirPath, bool filenamesOnly) {
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
