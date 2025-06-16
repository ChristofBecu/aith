#include "blacklist_file_manager.h"
#include "system_utils.h"
#include "file_utils.h"
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
    return FileUtils::fileExists(blacklistPath_);
}

/**
 * Reads all lines from the blacklist file
 */
std::vector<std::string> BlacklistFileManager::readAllLines() const {
    if (!exists()) {
        return {}; // Return empty vector if file doesn't exist
    }
    
    try {
        return FileUtils::readAllLines(blacklistPath_);
    } catch (const std::exception& e) {
        throw std::runtime_error("Could not read blacklist file: " + blacklistPath_ + " - " + e.what());
    }
}

/**
 * Writes all lines to the blacklist file (overwrites existing content)
 */
void BlacklistFileManager::writeAllLines(const std::vector<std::string>& lines) const {
    ensureConfigDirectoryExists();
    
    try {
        FileUtils::writeAllLines(blacklistPath_, lines);
    } catch (const std::exception& e) {
        throw std::runtime_error("Could not write to blacklist file: " + blacklistPath_ + " - " + e.what());
    }
}

/**
 * Appends a single line to the blacklist file
 */
void BlacklistFileManager::appendLine(const std::string& line) const {
    ensureConfigDirectoryExists();
    
    try {
        FileUtils::appendLine(blacklistPath_, line);
    } catch (const std::exception& e) {
        throw std::runtime_error("Could not append to blacklist file: " + blacklistPath_ + " - " + e.what());
    }
}

/**
 * Ensures the config directory exists, creating it if necessary
 */
void BlacklistFileManager::ensureConfigDirectoryExists() const {
    std::string configDir = getConfigDirectory();
    
    try {
        FileUtils::createDirectories(configDir);
    } catch (const std::exception& e) {
        throw std::runtime_error("Could not create config directory: " + configDir + " - " + e.what());
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
    
    return FileUtils::canRead(blacklistPath_);
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
    
    return FileUtils::canWrite(blacklistPath_);
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
