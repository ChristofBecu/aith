#include "api_models.h"
#include "blacklist_manager.h"
#include <iostream>
#include <sstream>
#include <ctime>

/**
 * Creates a formatted string representation of the model info.
 */
std::string ModelInfo::toString() const {
    std::string info = id;
    
    if (isBlacklisted) {
        info = "BLACKLISTED: " + info;
    }
    
    if (!ownedBy.empty()) {
        info += " | Owner: " + ownedBy;
    }
    
    if (created > 0) {
        std::string date = std::asctime(std::localtime(&created));
        // Remove trailing newline from asctime
        if (!date.empty() && date.back() == '\n') {
            date.pop_back();
        }
        info += " | Created: " + date;
    }
    
    return info;
}

/**
 * Constructs a chat request.
 */
ChatRequest::ChatRequest(const std::string& modelName, const Json::Value& chatMessages)
    : model(modelName), messages(chatMessages) {
}

/**
 * Converts the request to a JSON payload.
 */
Json::Value ChatRequest::toJson() const {
    Json::Value payload;
    payload["model"] = model;
    payload["messages"] = messages;
    return payload;
}

/**
 * Gets the model name.
 */
const std::string& ChatRequest::getModel() const {
    return model;
}

/**
 * Constructs a chat response from API response string.
 */
ChatResponse::ChatResponse(const std::string& jsonResponse) 
    : isError(true), errorMessage("Failed to parse response") {
    
    Json::Value data;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(jsonResponse);
    
    if (!Json::parseFromStream(reader, s, &data, &errs)) {
        errorMessage = "Error parsing JSON: " + errs;
        return;
    }
    
    // Check if response has expected format
    if (!data.isMember("choices") || !data["choices"].isArray() || data["choices"].empty()) {
        errorMessage = "Invalid API response format: missing or empty choices array";
        return;
    }
    
    const Json::Value& firstChoice = data["choices"][0];
    if (!firstChoice.isMember("message") || !firstChoice["message"].isMember("content")) {
        errorMessage = "Invalid API response format: missing message content";
        return;
    }
    
    // Successfully parsed
    content = firstChoice["message"]["content"].asString();
    isError = false;
    errorMessage.clear();
}

/**
 * Checks if the response represents an error.
 */
bool ChatResponse::hasError() const {
    return isError;
}

/**
 * Gets the error message if this is an error response.
 */
const std::string& ChatResponse::getErrorMessage() const {
    return errorMessage;
}

/**
 * Gets the content from the assistant's response.
 */
const std::string& ChatResponse::getContent() const {
    return content;
}

/**
 * Constructs a models list response from API response string.
 */
ModelsListResponse::ModelsListResponse(const std::string& jsonResponse, const std::string& provider)
    : isError(true), errorMessage("Failed to parse response") {
    
    Json::Value data;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(jsonResponse);
    
    if (!Json::parseFromStream(reader, s, &data, &errs)) {
        errorMessage = "Error parsing JSON: " + errs;
        return;
    }
    
    // Try to parse different response formats
    Json::Value modelsArray;
    
    // Check for OpenAI compatible format (data array)
    if (data.isMember("data") && data["data"].isArray()) {
        modelsArray = data["data"];
    }
    // Check for alternative format (models array)
    else if (data.isMember("models") && data["models"].isArray()) {
        modelsArray = data["models"];
    }
    else {
        errorMessage = "Unexpected response format: no recognized models array found";
        return;
    }
    
    // Parse each model
    for (const auto& modelJson : modelsArray) {
        if (!modelJson.isMember("id")) {
            continue; // Skip models without ID
        }
        
        ModelInfo model;
        model.id = modelJson["id"].asString();
        
        if (modelJson.isMember("owned_by")) {
            model.ownedBy = modelJson["owned_by"].asString();
        }
        
        if (modelJson.isMember("created") && modelJson["created"].isInt64()) {
            model.created = modelJson["created"].asInt64();
        }
        
        // Check if model is blacklisted
        model.isBlacklisted = BlacklistManager::isModelBlacklisted(provider, model.id);
        
        models.push_back(model);
    }
    
    // Successfully parsed
    isError = false;
    errorMessage.clear();
}

/**
 * Checks if the response represents an error.
 */
bool ModelsListResponse::hasError() const {
    return isError;
}

/**
 * Gets the error message if this is an error response.
 */
const std::string& ModelsListResponse::getErrorMessage() const {
    return errorMessage;
}

/**
 * Gets the list of models.
 */
const std::vector<ModelInfo>& ModelsListResponse::getModels() const {
    return models;
}

/**
 * Prints all models to stdout with formatting.
 */
void ModelsListResponse::printModels() const {
    for (const auto& model : models) {
        std::cout << model.toString() << std::endl;
    }
}
