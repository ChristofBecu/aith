#include "config_file_handler.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

std::string ConfigFileHandler::readValue(const std::string& configPath, const std::string& key) {
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

void ConfigFileHandler::writeValue(const std::string& configPath, const std::string& key, 
                                   const std::string& value) {
    // Read existing config
    std::vector<std::string> lines;
    std::ifstream inFile(configPath);
    
    bool keyFound = false;
    std::string line;
    
    if (inFile.is_open()) {
        while (std::getline(inFile, line)) {
            if (!line.empty() && line[0] != '#') {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string fileKey = line.substr(0, pos);
                    fileKey.erase(0, fileKey.find_first_not_of(" \t"));
                    fileKey.erase(fileKey.find_last_not_of(" \t") + 1);
                    
                    if (fileKey == key) {
                        line = key + "=" + value;
                        keyFound = true;
                    }
                }
            }
            lines.push_back(line);
        }
        inFile.close();
    }
    
    // If key wasn't found, add it
    if (!keyFound) {
        lines.push_back(key + "=" + value);
    }
    
    // Write back to file
    std::ofstream outFile(configPath);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open config file for writing: " + configPath);
    }
    
    for (const auto& configLine : lines) {
        outFile << configLine << std::endl;
    }
}
