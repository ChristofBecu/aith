#include "file_permissions.h"
#include "file_operations.h"
#include <fstream>

bool FilePermissions::canRead(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

bool FilePermissions::canWrite(const std::string& filePath) {
    if (FileOperations::exists(filePath)) {
        std::ofstream file(filePath, std::ios::app);
        return file.good();
    } else {
        // Try to create a file to test write permissions
        std::ofstream file(filePath);
        bool canWrite = file.good();
        file.close();
        if (canWrite) {
            FileOperations::remove(filePath); // Clean up test file
        }
        return canWrite;
    }
}
