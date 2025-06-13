#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <json/json.h>

/**
 * @brief Represents a model information from the API.
 */
struct ModelInfo {
    std::string id;
    std::string ownedBy;
    time_t created;
    bool isBlacklisted;
    
    ModelInfo() : created(0), isBlacklisted(false) {}
    
    /**
     * Creates a formatted string representation of the model info.
     * @return Formatted string with model details
     */
    std::string toString() const;
};

/**
 * @brief Request model for chat completion API calls.
 */
class ChatRequest {
private:
    std::string model;
    Json::Value messages;
    
public:
    /**
     * Constructs a chat request.
     * @param modelName The model to use for the chat
     * @param chatMessages The message history for the chat
     */
    ChatRequest(const std::string& modelName, const Json::Value& chatMessages);
    
    /**
     * Converts the request to a JSON payload.
     * @return Json::Value containing the request payload
     */
    Json::Value toJson() const;
    
    /**
     * Gets the model name.
     * @return The model name
     */
    const std::string& getModel() const;
};

/**
 * @brief Response model for chat completion API calls.
 */
class ChatResponse {
private:
    std::string content;
    bool isError;
    std::string errorMessage;
    
public:
    /**
     * Constructs a chat response from API response string.
     * @param jsonResponse The raw JSON response from the API
     */
    ChatResponse(const std::string& jsonResponse);
    
    /**
     * Checks if the response represents an error.
     * @return True if this is an error response
     */
    bool hasError() const;
    
    /**
     * Gets the error message if this is an error response.
     * @return The error message, or empty string if no error
     */
    const std::string& getErrorMessage() const;
    
    /**
     * Gets the content from the assistant's response.
     * @return The response content, or empty string if error
     */
    const std::string& getContent() const;
};

/**
 * @brief Response model for models list API calls.
 */
class ModelsListResponse {
private:
    std::vector<ModelInfo> models;
    bool isError;
    std::string errorMessage;
    
public:
    /**
     * Constructs a models list response from API response string.
     * @param jsonResponse The raw JSON response from the API
     * @param provider The provider name for blacklist checking
     */
    ModelsListResponse(const std::string& jsonResponse, const std::string& provider);
    
    /**
     * Checks if the response represents an error.
     * @return True if this is an error response
     */
    bool hasError() const;
    
    /**
     * Gets the error message if this is an error response.
     * @return The error message, or empty string if no error
     */
    const std::string& getErrorMessage() const;
    
    /**
     * Gets the list of models.
     * @return Vector of ModelInfo objects
     */
    const std::vector<ModelInfo>& getModels() const;
    
    /**
     * Prints all models to stdout with formatting.
     */
    void printModels() const;
};
