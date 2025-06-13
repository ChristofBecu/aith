#ifndef BENCHMARK_RESPONSE_H
#define BENCHMARK_RESPONSE_H

#include <string>
#include <json/json.h>

/**
 * Represents a benchmark response with parsing and validation logic
 * Encapsulates response analysis, error detection, and result extraction
 */
class BenchmarkResponse {
private:
    std::string rawResponse_;
    Json::Value parsedData_;
    bool parseSuccessful_;
    std::string parseError_;
    
    // Internal validation state
    bool hasValidStructure_;
    bool hasErrorField_;
    std::string errorMessage_;
    
    /**
     * Internal method to parse and analyze the response
     */
    void parseAndAnalyze();
    
public:
    /**
     * Constructor that parses the raw JSON response
     * @param jsonResponse Raw JSON response string from API
     */
    explicit BenchmarkResponse(const std::string& jsonResponse);
    
    /**
     * Checks if the response was successfully parsed
     * @return True if JSON parsing succeeded
     */
    bool isParseable() const { return parseSuccessful_; }
    
    /**
     * Checks if the response represents a successful API call
     * @return True if the response indicates success
     */
    bool isSuccessful() const;
    
    /**
     * Checks if the response has a valid chat completion structure
     * @return True if response has expected structure (choices, message, etc.)
     */
    bool hasValidChatStructure() const { return hasValidStructure_; }
    
    /**
     * Checks if the response contains an error field
     * @return True if response has an error field
     */
    bool hasError() const { return hasErrorField_; }
    
    /**
     * Gets the error message from the response or parse error
     * @return Error message string
     */
    std::string getErrorMessage() const;
    
    /**
     * Determines if this model should be blacklisted based on the response
     * @return True if model should be auto-blacklisted
     */
    bool shouldBlacklist() const;
    
    /**
     * Gets the blacklist reason if model should be blacklisted
     * @return Reason string for blacklisting
     */
    std::string getBlacklistReason() const;
    
    /**
     * Gets the content of the response message (if successful)
     * @return Message content string
     */
    std::string getMessageContent() const;
    
    /**
     * Gets the raw response string
     * @return Raw JSON response
     */
    const std::string& getRawResponse() const { return rawResponse_; }
    
    /**
     * Gets the parsed JSON data
     * @return Parsed JSON value
     */
    const Json::Value& getParsedData() const { return parsedData_; }
};

#endif // BENCHMARK_RESPONSE_H
