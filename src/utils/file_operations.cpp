#include "file_operations.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>

// File existence and basic operations
bool FileOperations::exists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

bool FileOperations::remove(const std::string& filePath) {
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

size_t FileOperations::removeMultiple(const std::vector<std::string>& filePaths) {
    size_t removedCount = 0;
    for (const auto& filePath : filePaths) {
        if (remove(filePath)) {
            removedCount++;
        }
    }
    return removedCount;
}

// File content operations
std::string FileOperations::read(const std::string& filePath) {
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FileOperations::write(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }
    
    file << content;
}

void FileOperations::appendLine(const std::string& filePath, const std::string& line) {
    std::ofstream file(filePath, std::ios::app);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for appending: " + filePath);
    }
    
    file << line << std::endl;
}

std::vector<std::string> FileOperations::readAllLines(const std::string& filePath) {
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

void FileOperations::writeAllLines(const std::string& filePath, const std::vector<std::string>& lines) {
    std::ofstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }
    
    for (const auto& line : lines) {
        file << line << std::endl;
    }
}

// File manipulation
bool FileOperations::rename(const std::string& oldPath, const std::string& newPath) {
    try {
        std::filesystem::rename(oldPath, newPath);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error renaming file from " << oldPath << " to " << newPath 
                  << ": " << e.what() << std::endl;
        return false;
    }
}

bool FileOperations::copy(const std::string& sourcePath, const std::string& destPath) {
    try {
        std::filesystem::copy_file(sourcePath, destPath, 
                                   std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error copying file from " << sourcePath << " to " << destPath 
                  << ": " << e.what() << std::endl;
        return false;
    }
}

// File metadata
std::size_t FileOperations::getSize(const std::string& filePath) {
    try {
        return std::filesystem::file_size(filePath);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get file size: " + filePath + " - " + e.what());
    }
}

std::string FileOperations::getExtension(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.extension().string();
}
