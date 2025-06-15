#include "blacklist_file_manager.h"
#include "system_utils.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

/**
 * Constructor that initializes the blacklist file path
 */
BlacklistFileManager::BlacklistFileManager() 
    : blacklistPath_(resolveBlacklistPath()) {
}

/**
 * Checks if the blacklist file exists
 */
bool BlacklistFileManager::exists() const {
    return std::filesystem::exists(blacklistPath_);
}

/**
 * Reads all lines from the blacklist file
 */
std::vector<std::string> BlacklistFileManager::readAllLines() const {
    std::vector<std::string> lines;
    
    if (!exists()) {
        return lines; // Return empty vector if file doesn't exist
    }
    
    std::ifstream file(blacklistPath_);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open blacklist file for reading: " + blacklistPath_);
    }
    
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

/**
 * Writes all lines to the blacklist file (overwrites existing content)
 */
void BlacklistFileManager::writeAllLines(const std::vector<std::string>& lines) const {
    ensureConfigDirectoryExists();
    
    std::ofstream file(blacklistPath_);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open blacklist file for writing: " + blacklistPath_);
    }
    
    for (const auto& line : lines) {
        file << line << std::endl;
    }
    
    if (file.fail()) {
        throw std::runtime_error("Failed to write to blacklist file: " + blacklistPath_);
    }
}

/**
 * Appends a single line to the blacklist file
 */
void BlacklistFileManager::appendLine(const std::string& line) const {
    ensureConfigDirectoryExists();
    
    std::ofstream file(blacklistPath_, std::ios::app);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open blacklist file for appending: " + blacklistPath_);
    }
    
    file << line << std::endl;
    
    if (file.fail()) {
        throw std::runtime_error("Failed to append to blacklist file: " + blacklistPath_);
    }
}

/**
 * Ensures the config directory exists, creating it if necessary
 */
void BlacklistFileManager::ensureConfigDirectoryExists() const {
    std::string configDir = getConfigDirectory();
    
    if (!std::filesystem::exists(configDir)) {
        try {
            std::filesystem::create_directories(configDir);
        } catch (const std::filesystem::filesystem_error& e) {
            throw std::runtime_error("Could not create config directory: " + configDir + 
                                   " - " + e.what());
        }
    }
}

/**
 * Gets the full path to the blacklist file
 */
std::string BlacklistFileManager::getBlacklistPath() const {
    return blacklistPath_;
}

/**
 * Checks if the blacklist file can be read
 */
bool BlacklistFileManager::canRead() const {
    if (!exists()) {
        return false;
    }
    
    std::ifstream file(blacklistPath_);
    return file.good();
}

/**
 * Checks if the blacklist file can be written
 */
bool BlacklistFileManager::canWrite() const {
    // Try to ensure directory exists first
    try {
        ensureConfigDirectoryExists();
    } catch (const std::runtime_error&) {
        return false;
    }
    
    // Try to open file for writing (this will create it if it doesn't exist)
    std::ofstream file(blacklistPath_, std::ios::app);
    return file.good();
}

/**
 * Resolves the blacklist file path using system utilities
 */
std::string BlacklistFileManager::resolveBlacklistPath() const {
    return getConfigDirectory() + "/blacklist";
}

/**
 * Gets the config directory path
 */
std::string BlacklistFileManager::getConfigDirectory() const {
    std::string home = SystemUtils::getEnvVar("HOME");
    return home + "/.config/aith";
}
