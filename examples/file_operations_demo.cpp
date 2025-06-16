#include <iostream>
#include <stdexcept>
#include "file_operations.h"
#include "directory_operations.h"
#include "temp_file_manager.h"
#include "json_file_handler.h"
#include "config_file_handler.h"
#include "file_permissions.h"

/**
 * @brief Demonstration of the new specialized file operation classes
 * 
 * This example shows how to use each of the new specialized classes
 * instead of the monolithic FileUtils class.
 */

void demonstrateFileOperations() {
    std::cout << "=== FileOperations Demo ===" << std::endl;
    
    try {
        // Create a test file
        FileOperations::write("/tmp/test.txt", "Hello, World!");
        
        // Check if it exists
        if (FileOperations::exists("/tmp/test.txt")) {
            std::cout << "✓ File created successfully" << std::endl;
        }
        
        // Read the content
        std::string content = FileOperations::read("/tmp/test.txt");
        std::cout << "✓ File content: " << content << std::endl;
        
        // Get file size and extension
        std::cout << "✓ File size: " << FileOperations::getSize("/tmp/test.txt") << " bytes" << std::endl;
        std::cout << "✓ File extension: " << FileOperations::getExtension("/tmp/test.txt") << std::endl;
        
        // Clean up
        FileOperations::remove("/tmp/test.txt");
        std::cout << "✓ File removed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in FileOperations demo: " << e.what() << std::endl;
    }
}

void demonstrateDirectoryOperations() {
    std::cout << "\n=== DirectoryOperations Demo ===" << std::endl;
    
    try {
        // Create a test directory
        DirectoryOperations::create("/tmp/test_dir");
        
        // Check if it exists
        if (DirectoryOperations::exists("/tmp/test_dir")) {
            std::cout << "✓ Directory created successfully" << std::endl;
        }
        
        // Create some test files in it
        FileOperations::write("/tmp/test_dir/file1.txt", "File 1");
        FileOperations::write("/tmp/test_dir/file2.txt", "File 2");
        
        // List directory contents
        auto files = DirectoryOperations::list("/tmp/test_dir");
        std::cout << "✓ Directory contents:" << std::endl;
        for (const auto& file : files) {
            std::cout << "  - " << file << std::endl;
        }
        
        // Clean up
        FileOperations::remove("/tmp/test_dir/file1.txt");
        FileOperations::remove("/tmp/test_dir/file2.txt");
        std::cout << "✓ Test files removed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in DirectoryOperations demo: " << e.what() << std::endl;
    }
}

void demonstrateTempFileManager() {
    std::cout << "\n=== TempFileManager Demo ===" << std::endl;
    
    try {
        // Create a temporary file
        std::string tempFile = TempFileManager::createTempFile("Temporary content", "demo");
        std::cout << "✓ Temporary file created: " << tempFile << std::endl;
        
        // Create a temporary JSON file
        std::string jsonContent = R"({"name": "test", "value": 42})";
        std::string tempJsonFile = TempFileManager::createTempJsonFile(jsonContent, "demo");
        std::cout << "✓ Temporary JSON file created: " << tempJsonFile << std::endl;
        
        // Create temp file with timestamp
        std::string timestampFile = TempFileManager::createTempFileWithTimestamp("Timestamp content", "demo");
        std::cout << "✓ Timestamp file created: " << timestampFile << std::endl;
        
        // Show temp directory
        std::cout << "✓ Temp directory: " << TempFileManager::getTempDirectory() << std::endl;
        
        // Clean up
        FileOperations::remove(tempFile);
        FileOperations::remove(tempJsonFile);
        FileOperations::remove(timestampFile);
        std::cout << "✓ Temporary files removed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in TempFileManager demo: " << e.what() << std::endl;
    }
}

void demonstrateJsonFileHandler() {
    std::cout << "\n=== JsonFileHandler Demo ===" << std::endl;
    
    try {
        // Create JSON data
        Json::Value jsonData;
        jsonData["name"] = "John Doe";
        jsonData["age"] = 30;
        jsonData["languages"] = Json::Value(Json::arrayValue);
        jsonData["languages"].append("C++");
        jsonData["languages"].append("Python");
        jsonData["languages"].append("JavaScript");
        
        // Write to file
        std::string jsonFile = "/tmp/test.json";
        JsonFileHandler::write(jsonFile, jsonData);
        std::cout << "✓ JSON file written" << std::endl;
        
        // Read back from file
        Json::Value readData = JsonFileHandler::read(jsonFile);
        std::cout << "✓ JSON file read back:" << std::endl;
        std::cout << "  Name: " << readData["name"].asString() << std::endl;
        std::cout << "  Age: " << readData["age"].asInt() << std::endl;
        std::cout << "  Languages: ";
        for (const auto& lang : readData["languages"]) {
            std::cout << lang.asString() << " ";
        }
        std::cout << std::endl;
        
        // Clean up
        FileOperations::remove(jsonFile);
        std::cout << "✓ JSON file removed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in JsonFileHandler demo: " << e.what() << std::endl;
    }
}

void demonstrateConfigFileHandler() {
    std::cout << "\n=== ConfigFileHandler Demo ===" << std::endl;
    
    try {
        std::string configFile = "/tmp/test.config";
        
        // Write some config values
        ConfigFileHandler::writeValue(configFile, "database_host", "localhost");
        ConfigFileHandler::writeValue(configFile, "database_port", "5432");
        ConfigFileHandler::writeValue(configFile, "debug_mode", "true");
        std::cout << "✓ Config values written" << std::endl;
        
        // Read config values
        std::string host = ConfigFileHandler::readValue(configFile, "database_host");
        std::string port = ConfigFileHandler::readValue(configFile, "database_port");
        std::string debug = ConfigFileHandler::readValue(configFile, "debug_mode");
        
        std::cout << "✓ Config values read:" << std::endl;
        std::cout << "  Database Host: " << host << std::endl;
        std::cout << "  Database Port: " << port << std::endl;
        std::cout << "  Debug Mode: " << debug << std::endl;
        
        // Update a value
        ConfigFileHandler::writeValue(configFile, "database_port", "3306");
        std::string newPort = ConfigFileHandler::readValue(configFile, "database_port");
        std::cout << "✓ Updated port: " << newPort << std::endl;
        
        // Clean up
        FileOperations::remove(configFile);
        std::cout << "✓ Config file removed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in ConfigFileHandler demo: " << e.what() << std::endl;
    }
}

void demonstrateFilePermissions() {
    std::cout << "\n=== FilePermissions Demo ===" << std::endl;
    
    try {
        std::string testFile = "/tmp/permission_test.txt";
        
        // Create a test file
        FileOperations::write(testFile, "Permission test content");
        
        // Check permissions
        bool canRead = FilePermissions::canRead(testFile);
        bool canWrite = FilePermissions::canWrite(testFile);
        
        std::cout << "✓ File permissions:" << std::endl;
        std::cout << "  Can read: " << (canRead ? "Yes" : "No") << std::endl;
        std::cout << "  Can write: " << (canWrite ? "Yes" : "No") << std::endl;
        
        // Clean up
        FileOperations::remove(testFile);
        std::cout << "✓ Permission test file removed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in FilePermissions demo: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "🎯 File Operations Architecture Demo" << std::endl;
    std::cout << "Demonstrating the new specialized classes instead of monolithic FileUtils" << std::endl;
    std::cout << "=====================================================================" << std::endl;
    
    demonstrateFileOperations();
    demonstrateDirectoryOperations();
    demonstrateTempFileManager();
    demonstrateJsonFileHandler();
    demonstrateConfigFileHandler();
    demonstrateFilePermissions();
    
    std::cout << "\n🎉 All demos completed successfully!" << std::endl;
    std::cout << "The new architecture provides clean, focused classes for each responsibility." << std::endl;
    
    return 0;
}
