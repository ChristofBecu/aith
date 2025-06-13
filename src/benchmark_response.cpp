#include "benchmark_response.h"
#include <sstream>

/**
 * Constructor that parses the raw JSON response
 */
BenchmarkResponse::BenchmarkResponse(const std::string& jsonResponse)
    : rawResponse_(jsonResponse), parseSuccessful_(false), hasValidStructure_(false), hasErrorField_(false) {
    parseAndAnalyze();
}

/**
 * Internal method to parse and analyze the response
 */
void BenchmarkResponse::parseAndAnalyze() {
    // Parse JSON
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(rawResponse_);
    
    parseSuccessful_ = Json::parseFromStream(reader, s, &parsedData_, &errs);
    
    if (!parseSuccessful_) {
        parseError_ = "Failed to parse JSON response: " + errs;
        return;
    }
    
    // Check for error field
    if (parsedData_.isMember("error")) {
        hasErrorField_ = true;
        if (parsedData_["error"].isMember("message")) {
            errorMessage_ = parsedData_["error"]["message"].asString();
        } else {
            errorMessage_ = "API returned an error";
        }
        return;
    }
    
    // Check for valid chat completion structure
    if (parsedData_.isMember("choices") && 
        parsedData_["choices"].isArray() && 
        !parsedData_["choices"].empty() &&
        parsedData_["choices"][0].isMember("message")) {
        hasValidStructure_ = true;
    }
}

/**
 * Checks if the response represents a successful API call
 */
bool BenchmarkResponse::isSuccessful() const {
    return parseSuccessful_ && !hasErrorField_ && hasValidStructure_;
}

/**
 * Gets the error message from the response or parse error
 */
std::string BenchmarkResponse::getErrorMessage() const {
    if (!parseSuccessful_) {
        return parseError_;
    }
    if (hasErrorField_) {
        return errorMessage_;
    }
    if (!hasValidStructure_) {
        return "Invalid API response format";
    }
    return ""; // No error
}

/**
 * Determines if this model should be blacklisted based on the response
 */
bool BenchmarkResponse::shouldBlacklist() const {
    // Blacklist if response structure is invalid (doesn't support chat completions)
    return parseSuccessful_ && !hasErrorField_ && !hasValidStructure_;
}

/**
 * Gets the blacklist reason if model should be blacklisted
 */
std::string BenchmarkResponse::getBlacklistReason() const {
    if (shouldBlacklist()) {
        return "Auto-blacklisted: Invalid API response format";
    }
    return "";
}

/**
 * Gets the content of the response message (if successful)
 */
std::string BenchmarkResponse::getMessageContent() const {
    if (!isSuccessful()) {
        return "";
    }
    
    const Json::Value& choices = parsedData_["choices"];
    if (!choices.empty() && 
        choices[0].isMember("message") && 
        choices[0]["message"].isMember("content")) {
        return choices[0]["message"]["content"].asString();
    }
    
    return "";
}
