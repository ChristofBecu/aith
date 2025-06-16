#include "http_client.h"
#include "system_utils.h"
#include "temp_file_manager.h"
#include "file_operations.h"
#include <httplib.h>
#include <sstream>
#include <stdexcept>
#include <regex>

/**
 * Legacy curl-based HTTP GET request.
 * @deprecated Use getRequest() for better performance and error handling
 */
std::string HttpClient::get(const std::string& url, const std::string& apiKey) {
    std::string command = "curl -s -X GET \"" + url + "\" -H \"Authorization: Bearer " + apiKey + "\"";
    return SystemUtils::exec(command.c_str());
}

/**
 * Legacy curl-based HTTP POST request with JSON payload.
 * @deprecated Use postRequest() for better performance and error handling
 */
std::string HttpClient::post(const std::string& url, const std::string& apiKey, const Json::Value& payload) {
    // Convert JSON payload to string for temporary file creation
    Json::StreamWriterBuilder writer;
    std::string payloadJson = Json::writeString(writer, payload);
    
    // Create temporary file with JSON content using TempFileManager
    std::string tempFile = TempFileManager::createTempFileWithTimestamp(payloadJson, "aith_payload");
    
    std::string command = "curl -s -X POST \"" + url + "\" " +
                         "-H \"Authorization: Bearer " + apiKey + "\" " +
                         "-H \"Content-Type: application/json\" " +
                         "-d @" + tempFile;
    
    std::string response = SystemUtils::exec(command.c_str());
    
    // Clean up the temporary file using FileOperations
    // Note: TempFileManager doesn't have a remove method, we'll use FileOperations for cleanup
    FileOperations::remove(tempFile);
    
    return response;
}

// ====================================================================
// Modern httplib-based HTTP client methods
// ====================================================================

/**
 * Modern HTTP GET request using httplib library.
 * Offers better performance, error handling, and security.
 */
std::string HttpClient::getRequest(const std::string& url, const std::string& apiKey) {
    // httplib::Client can handle both HTTP and HTTPS automatically based on URL
    httplib::Client client(url);
    configureClient(client);
    
    // Set headers
    httplib::Headers headers = {
        {"Authorization", "Bearer " + apiKey},
        {"User-Agent", "aith/1.0"}
    };
    
    // Make the request (path is "/" since we pass full URL to constructor)
    auto response = client.Get("/", headers);
    validateResponse(response, "GET");
    
    return response->body;
}

/**
 * Modern HTTP POST request with JSON payload using httplib library.
 * Offers better performance, error handling, and security.
 */
std::string HttpClient::postRequest(const std::string& url, const std::string& apiKey, 
                                   const Json::Value& payload) {
    // httplib::Client can handle both HTTP and HTTPS automatically based on URL
    httplib::Client client(url);
    configureClient(client);
    
    // Convert JSON payload to string
    Json::StreamWriterBuilder writer;
    std::string jsonData = Json::writeString(writer, payload);
    
    // Set headers
    httplib::Headers headers = {
        {"Authorization", "Bearer " + apiKey},
        {"Content-Type", "application/json"},
        {"User-Agent", "aith/1.0"}
    };
    
    // Make the request (path is "/" since we pass full URL to constructor)
    auto response = client.Post("/", headers, jsonData, "application/json");
    validateResponse(response, "POST");
    
    return response->body;
}

// ====================================================================
// Private utility methods for httplib implementation
// ====================================================================

/**
 * Validates HTTP response and throws appropriate errors.
 */
void HttpClient::validateResponse(const httplib::Result& response, 
                                 const std::string& operation) {
    if (!response) {
        throw std::runtime_error("HTTP " + operation + " request failed: Connection error or timeout");
    }
    
    // Check for HTTP error status codes
    if (response->status >= 400) {
        std::string errorMsg = "HTTP " + operation + " request failed with status " + 
                              std::to_string(response->status);
        
        // Add more specific error messages for common status codes
        switch (response->status) {
            case 400:
                errorMsg += " (Bad Request)";
                break;
            case 401:
                errorMsg += " (Unauthorized - check API key)";
                break;
            case 403:
                errorMsg += " (Forbidden)";
                break;
            case 404:
                errorMsg += " (Not Found)";
                break;
            case 429:
                errorMsg += " (Too Many Requests - rate limited)";
                break;
            case 500:
                errorMsg += " (Internal Server Error)";
                break;
            case 502:
                errorMsg += " (Bad Gateway)";
                break;
            case 503:
                errorMsg += " (Service Unavailable)";
                break;
        }
        
        // Include response body if available for debugging
        if (!response->body.empty()) {
            errorMsg += ". Response: " + response->body;
        }
        
        throw std::runtime_error(errorMsg);
    }
}
