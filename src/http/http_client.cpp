#include "http_client.h"
#include "system_utils.h"
#include "file_utils.h"
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
    // Convert JSON payload to string for temporary file creation
    Json::StreamWriterBuilder writer;
    std::string payloadJson = Json::writeString(writer, payload);
    
    // Create temporary file with JSON content using FileUtils
    std::string tempFile = FileUtils::createTempFileWithTimestamp(payloadJson, "aith_payload");
    
    std::string command = "curl -s -X POST \"" + url + "\" " +
                         "-H \"Authorization: Bearer " + apiKey + "\" " +
                         "-H \"Content-Type: application/json\" " +
                         "-d @" + tempFile;
    
    std::string response = SystemUtils::exec(command.c_str());
    
    // Clean up the temporary file using FileUtils
    FileUtils::removeFile(tempFile);
    
    return response;
}
