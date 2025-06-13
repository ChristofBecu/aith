#pragma once

#include <string>
#include <json/json.h>

/**
 * @brief HTTP client for making API requests.
 * 
 * This class provides static methods for making HTTP requests using curl.
 * It abstracts the low-level curl command construction and execution,
 * providing a clean interface for GET and POST operations with authentication.
 */
class HttpClient {
public:
    /**
     * Performs an HTTP GET request.
     * @param url The complete URL to make the GET request to
     * @param apiKey The API key for Bearer token authentication
     * @return The response body as a string
     */
    static std::string get(const std::string& url, const std::string& apiKey);
    
    /**
     * Performs an HTTP POST request with JSON payload.
     * @param url The complete URL to make the POST request to
     * @param apiKey The API key for Bearer token authentication
     * @param payload The JSON payload to send in the request body
     * @return The response body as a string
     */
    static std::string post(const std::string& url, const std::string& apiKey, const Json::Value& payload);

private:
    /**
     * Writes JSON payload to a temporary file and returns the file path.
     * @param payload The JSON payload to write
     * @return The path to the temporary file containing the JSON payload
     */
    static std::string writeJsonToTempFile(const Json::Value& payload);
    
    /**
     * Removes a temporary file if it exists.
     * @param filePath The path to the file to remove
     */
    static void cleanupTempFile(const std::string& filePath);
};
