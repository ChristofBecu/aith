#ifndef BENCHMARK_REQUEST_H
#define BENCHMARK_REQUEST_H

#include <string>
#include <json/json.h>

/**
 * Represents a benchmark request with all necessary data for testing a model
 * Encapsulates request construction, validation, and serialization
 */
class BenchmarkRequest {
private:
    std::string model_;
    std::string prompt_;
    int maxTokens_;
    
public:
    /**
     * Constructor for benchmark request
     * @param model The model name to test
     * @param prompt The test prompt to send
     * @param maxTokens Maximum tokens for the response
     */
    BenchmarkRequest(const std::string& model, const std::string& prompt, int maxTokens);
    
    /**
     * Validates the request parameters
     * @return True if request is valid, false otherwise
     */
    bool isValid() const;
    
    /**
     * Get validation error message if request is invalid
     * @return Error message string
     */
    std::string getValidationError() const;
    
    /**
     * Converts the request to JSON format
     * @return JSON value representing the request
     */
    Json::Value toJson() const;
    
    /**
     * Converts the request to JSON string
     * @return JSON string representation of the request
     */
    std::string toJsonString() const;
    
    /**
     * Creates a temporary file with the request JSON
     * @param sanitizedModelName Sanitized model name for file naming
     * @return Path to the created temporary file
     */
    std::string createTempFile(const std::string& sanitizedModelName) const;
    
    // Getters
    const std::string& getModel() const { return model_; }
    const std::string& getPrompt() const { return prompt_; }
    int getMaxTokens() const { return maxTokens_; }
    
    // Setters (if needed for configuration)
    void setMaxTokens(int maxTokens) { maxTokens_ = maxTokens; }
};

#endif // BENCHMARK_REQUEST_H
