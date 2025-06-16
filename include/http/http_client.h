#pragma once

#include <string>
#include <json/json.h>
#include <httplib.h>
#include <memory>

/**
 * @brief Modern HTTP client for making API requests.
 * 
 * This class provides static methods for making HTTP requests using the
 * high-performance httplib library. Offers excellent performance, detailed
 * error handling, automatic SSL/TLS support, and enhanced security.
 */
class HttpClient {
public:
    // HTTP client methods using httplib library
    /**
     * Performs an HTTP GET request using httplib library.
     * Offers excellent performance, error handling, and security.
     * @param url The complete URL to make the GET request to
     * @param apiKey The API key for Bearer token authentication
     * @return The response body as a string
     * @throws std::runtime_error if the request fails
     * @throws std::invalid_argument if the URL format is invalid
     */
    static std::string get(const std::string& url, const std::string& apiKey);
    
    /**
     * Performs an HTTP POST request with JSON payload using httplib library.
     * Offers excellent performance, error handling, and security.
     * @param url The complete URL to make the POST request to
     * @param apiKey The API key for Bearer token authentication
     * @param payload The JSON payload to send in the request body
     * @return The response body as a string
     * @throws std::runtime_error if the request fails
     * @throws std::invalid_argument if the URL format is invalid
     */
    static std::string post(const std::string& url, const std::string& apiKey, 
                           const Json::Value& payload);

private:
    // HTTP client configuration constants
    static constexpr int CONNECTION_TIMEOUT_SECONDS = 30;
    static constexpr int READ_TIMEOUT_SECONDS = 60;
    static constexpr int WRITE_TIMEOUT_SECONDS = 60;

    // Utility methods for httplib implementation
    /**
     * Configures HTTP client with timeouts and performance settings.
     * Template function to work with both Client and SSLClient.
     * @param client The HTTP client to configure
     */
    template<typename ClientType>
    static void configureClient(ClientType& client);

    /**
     * Validates HTTP response and throws appropriate errors.
     * @param response The HTTP response to validate
     * @param operation The operation name for error messages (e.g., "GET", "POST")
     * @throws std::runtime_error if response indicates failure
     */
    static void validateResponse(const httplib::Result& response, 
                               const std::string& operation);
    
    /**
     * Parses a full URL into base URL and path components.
     * @param url The full URL to parse
     * @return A pair containing [baseUrl, path]
     * @throws std::invalid_argument if URL format is invalid
     */
    static std::pair<std::string, std::string> parseUrl(const std::string& url);
};

// Template method implementations
template<typename ClientType>
void HttpClient::configureClient(ClientType& client) {
    // Configure timeouts for reliability
    client.set_connection_timeout(CONNECTION_TIMEOUT_SECONDS, 0);
    client.set_read_timeout(READ_TIMEOUT_SECONDS, 0);
    client.set_write_timeout(WRITE_TIMEOUT_SECONDS, 0);
    
    // Enable keep-alive for better performance
    client.set_keep_alive(true);
    
    // Disable compression to avoid JSON parsing issues with APIs
    // Many APIs don't handle compressed request bodies properly
    client.set_compress(false);
}
