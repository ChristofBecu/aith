#include "benchmark_request.h"
#include "file_utils.h"
#include "string_utils.h"
#include <json/json.h>

/**
 * Constructor for benchmark request
 */
BenchmarkRequest::BenchmarkRequest(const std::string& model, const std::string& prompt, int maxTokens)
    : model_(model), prompt_(prompt), maxTokens_(maxTokens) {
}

/**
 * Validates the request parameters
 */
bool BenchmarkRequest::isValid() const {
    return !model_.empty() && 
           !StringUtils::isEmptyOrWhitespace(prompt_) && 
           maxTokens_ > 0 && maxTokens_ <= 4096; // Reasonable token limit
}

/**
 * Get validation error message if request is invalid
 */
std::string BenchmarkRequest::getValidationError() const {
    if (model_.empty()) {
        return "Model name cannot be empty";
    }
    if (StringUtils::isEmptyOrWhitespace(prompt_)) {
        return "Prompt cannot be empty or whitespace only";
    }
    if (maxTokens_ <= 0) {
        return "Max tokens must be positive";
    }
    if (maxTokens_ > 4096) {
        return "Max tokens cannot exceed 4096";
    }
    return ""; // No error
}

/**
 * Converts the request to JSON format
 */
Json::Value BenchmarkRequest::toJson() const {
    Json::Value payload;
    payload["model"] = model_;
    
    // Build messages array
    Json::Value messages(Json::arrayValue);
    Json::Value userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = prompt_;
    messages.append(userMessage);
    payload["messages"] = messages;
    
    // Set max tokens
    payload["max_tokens"] = maxTokens_;
    
    return payload;
}

/**
 * Converts the request to JSON string
 */
std::string BenchmarkRequest::toJsonString() const {
    Json::StreamWriterBuilder writer;
    writer["indentation"] = ""; // Compact JSON
    return Json::writeString(writer, toJson());
}

/**
 * Creates a temporary file with the request JSON
 */
std::string BenchmarkRequest::createTempFile(const std::string& sanitizedModelName) const {
    std::string jsonContent = toJsonString();
    return FileUtils::createTempJsonFile(jsonContent, "benchmark_payload_" + sanitizedModelName);
}
