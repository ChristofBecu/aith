#include "string_utils.h"
#include <algorithm>
#include <cctype>

/**
 * Sanitizes a string for safe use in file paths
 * Replaces problematic characters with safe alternatives
 */
std::string StringUtils::sanitizeForFilename(const std::string& input) {
    std::string sanitized = input;
    
    // Replace problematic characters with safe alternatives
    for (char &c : sanitized) {
        switch (c) {
            case '/':  c = '_'; break;  // Directory separator
            case ':':  c = '-'; break;  // Can cause issues on some systems
            case ' ':  c = '_'; break;  // Spaces in filenames
            case '\\': c = '_'; break;  // Windows directory separator
            case '?':  c = '_'; break;  // Query character
            case '*':  c = '_'; break;  // Wildcard
            case '<':  c = '_'; break;  // Redirection
            case '>':  c = '_'; break;  // Redirection
            case '|':  c = '_'; break;  // Pipe
            case '"':  c = '_'; break;  // Quote
            // Keep other characters as-is (alphanumeric, -, _, .)
        }
    }
    
    return sanitized;
}

/**
 * Trims whitespace from both ends of a string
 */
std::string StringUtils::trim(const std::string& input) {
    size_t start = input.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = input.find_last_not_of(" \t\n\r\f\v");
    return input.substr(start, end - start + 1);
}

/**
 * Converts string to lowercase
 */
std::string StringUtils::toLowerCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

/**
 * Checks if a string is empty or contains only whitespace
 */
bool StringUtils::isEmptyOrWhitespace(const std::string& input) {
    return input.find_first_not_of(" \t\n\r\f\v") == std::string::npos;
}
