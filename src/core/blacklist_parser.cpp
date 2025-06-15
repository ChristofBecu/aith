#include "blacklist_parser.h"
#include <sstream>
#include <ctime>
#include <algorithm>
#include <iostream>

/**
 * Parses a single blacklist line into its components.
 */
ParsedBlacklistEntry BlacklistParser::parseLine(const std::string& line) {
    ParsedBlacklistEntry entry;
    entry.rawLine = line;
    entry.isValid = false;
    
    // Skip empty lines and comments
    if (isEmptyLine(line) || isCommentLine(line)) {
        return entry;
    }
    
    // Separate main content from comment
    std::string mainPart, commentPart;
    separateMainAndComment(line, mainPart, commentPart);
    
    // Parse the main part (provider | model | reason)
    std::vector<std::string> parts = splitByPipeDelimiter(mainPart);
    
    // Need at least provider and model
    if (parts.size() >= 2) {
        entry.provider = parts[0];
        entry.model = parts[1];
        
        // Add reason if present
        if (parts.size() >= 3) {
            entry.reason = parts[2];
        }
        
        // Extract timestamp from comment if present
        if (!commentPart.empty()) {
            entry.timestamp = extractTimestamp(commentPart);
        }
        
        // Validate the parsed components
        if (isValidProvider(entry.provider) && isValidModel(entry.model)) {
            entry.isValid = true;
        }
    }
    
    return entry;
}

/**
 * Formats a blacklist entry for storage in the file.
 */
std::string BlacklistParser::formatEntry(const std::string& provider, 
                                        const std::string& model,
                                        const std::string& reason,
                                        const std::string& timestamp) {
    std::string formattedEntry = provider + " | " + model;
    
    if (!reason.empty()) {
        formattedEntry += " | " + reason;
    }
    
    std::string entryTimestamp = timestamp.empty() ? getCurrentTimestamp() : timestamp;
    formattedEntry += " # Added: " + entryTimestamp;
    
    return formattedEntry;
}

/**
 * Splits a line by pipe delimiter and trims each part.
 */
std::vector<std::string> BlacklistParser::splitByPipeDelimiter(const std::string& line) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string part;
    
    while (std::getline(ss, part, '|')) {
        parts.push_back(trimWhitespace(part));
    }
    
    return parts;
}

/**
 * Trims whitespace from the beginning and end of a string.
 */
std::string BlacklistParser::trimWhitespace(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

/**
 * Extracts timestamp from a comment line.
 */
std::string BlacklistParser::extractTimestamp(const std::string& commentPart) {
    // Look for "Added:" or "Added on " patterns
    size_t addedPos = commentPart.find("Added:");
    if (addedPos != std::string::npos) {
        std::string timestamp = commentPart.substr(addedPos + 6); // "Added:" is 6 chars
        return trimWhitespace(timestamp);
    }
    
    size_t addedOnPos = commentPart.find("Added on ");
    if (addedOnPos != std::string::npos) {
        std::string timestamp = commentPart.substr(addedOnPos + 9); // "Added on " is 9 chars
        return trimWhitespace(timestamp);
    }
    
    return "";
}

/**
 * Generates a current timestamp string.
 */
std::string BlacklistParser::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::string timestamp = std::ctime(&now);
    // Remove the newline character from ctime output
    if (!timestamp.empty() && timestamp.back() == '\n') {
        timestamp.pop_back();
    }
    return timestamp;
}

/**
 * Checks if a line is a comment line (starts with #).
 */
bool BlacklistParser::isCommentLine(const std::string& line) {
    std::string trimmed = trimWhitespace(line);
    return !trimmed.empty() && trimmed[0] == '#';
}

/**
 * Checks if a line is empty or contains only whitespace.
 */
bool BlacklistParser::isEmptyLine(const std::string& line) {
    return trimWhitespace(line).empty();
}

/**
 * Validates that a provider name is acceptable.
 */
bool BlacklistParser::isValidProvider(const std::string& provider) {
    return !provider.empty() && provider.find('|') == std::string::npos && 
           provider.find('#') == std::string::npos;
}

/**
 * Validates that a model name is acceptable.
 */
bool BlacklistParser::isValidModel(const std::string& model) {
    return !model.empty() && model.find('|') == std::string::npos && 
           model.find('#') == std::string::npos;
}

/**
 * Separates the main content from comment in a line.
 */
void BlacklistParser::separateMainAndComment(const std::string& line, 
                                           std::string& mainPart, 
                                           std::string& commentPart) {
    size_t commentPos = line.find('#');
    if (commentPos != std::string::npos) {
        mainPart = trimWhitespace(line.substr(0, commentPos));
        commentPart = trimWhitespace(line.substr(commentPos + 1));
    } else {
        mainPart = trimWhitespace(line);
        commentPart = "";
    }
}
