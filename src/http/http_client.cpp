#include "http_client.h"
#include <httplib.h>
#include <stdexcept>
#include <regex>

// ====================================================================
// HTTP client methods using httplib library
// ====================================================================

/**
 * HTTP GET request using httplib library.
 * Offers excellent performance, error handling, and security.
 */
std::string HttpClient::get(const std::string& url, const std::string& apiKey) {
    // Parse URL to extract base URL and path
    auto [baseUrl, path] = parseUrl(url);
    
    // Create HTTP client with base URL only
    httplib::Client client(baseUrl);
    configureClient(client);
    
    // Set headers
    httplib::Headers headers = {
        {"Authorization", "Bearer " + apiKey},
        {"User-Agent", "aith/1.0"}
    };
    
    // Make the request with the correct path
    auto response = client.Get(path, headers);
    validateResponse(response, "GET");
    
    return response->body;
}

/**
 * HTTP POST request with JSON payload using httplib library.
 * Offers excellent performance, error handling, and security.
 */
std::string HttpClient::post(const std::string& url, const std::string& apiKey, 
                            const Json::Value& payload) {
    // Parse URL to extract base URL and path
    auto [baseUrl, path] = parseUrl(url);
    
    // Create HTTP client with base URL only
    httplib::Client client(baseUrl);
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
    
    // Make the request with the correct path
    auto response = client.Post(path, headers, jsonData, "application/json");
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

/**
 * Parses a full URL into base URL and path components.
 * @param url The full URL to parse
 * @return A pair containing [baseUrl, path]
 */
std::pair<std::string, std::string> HttpClient::parseUrl(const std::string& url) {
    // Regular expression to parse URL components
    // Matches: scheme://host[:port][/path]
    std::regex urlRegex(R"(^(https?://)([^/]+)(/.*)?$)");
    std::smatch matches;
    
    if (!std::regex_match(url, matches, urlRegex)) {
        throw std::invalid_argument("Invalid URL format: " + url);
    }
    
    std::string scheme = matches[1].str();      // "http://" or "https://"
    std::string host = matches[2].str();        // "example.com" or "example.com:8080"
    std::string path = matches[3].str();        // "/path" or empty
    
    // If no path specified, default to "/"
    if (path.empty()) {
        path = "/";
    }
    
    std::string baseUrl = scheme + host;
    
    return std::make_pair(baseUrl, path);
}
