#include "http_client.h"
#include "system_utils.h"
#include <fstream>
#include <filesystem>
#include <sstream>

/**
 * Performs an HTTP GET request.
 */
std::string HttpClient::get(const std::string& url, const std::string& apiKey) {
    std::string command = "curl -s -X GET \"" + url + "\" -H \"Authorization: Bearer " + apiKey + "\"";
    return SystemUtils::exec(command.c_str());
}

/**
 * Performs an HTTP POST request with JSON payload.
 */
std::string HttpClient::post(const std::string& url, const std::string& apiKey, const Json::Value& payload) {
    std::string tempFile = writeJsonToTempFile(payload);
    
    std::string command = "curl -s -X POST \"" + url + "\" " +
                         "-H \"Authorization: Bearer " + apiKey + "\" " +
                         "-H \"Content-Type: application/json\" " +
                         "-d @" + tempFile;
    
    std::string response = SystemUtils::exec(command.c_str());
    
    // Clean up the temporary file
    cleanupTempFile(tempFile);
    
    return response;
}

/**
 * Writes JSON payload to a temporary file and returns the file path.
 */
std::string HttpClient::writeJsonToTempFile(const Json::Value& payload) {
    std::string tempFile = "/tmp/aith_payload_" + std::to_string(std::time(nullptr)) + ".json";
    
    Json::StreamWriterBuilder writer;
    std::string payloadJson = Json::writeString(writer, payload);
    
    std::ofstream outFile(tempFile);
    outFile << payloadJson;
    outFile.close();
    
    return tempFile;
}

/**
 * Removes a temporary file if it exists.
 */
void HttpClient::cleanupTempFile(const std::string& filePath) {
    if (std::filesystem::exists(filePath)) {
        std::filesystem::remove(filePath);
    }
}
