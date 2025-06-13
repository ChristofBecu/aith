#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>

/**
 * String utility functions for common string operations
 * Provides reusable string manipulation and validation methods
 */
class StringUtils {
public:
    /**
     * Sanitizes a string for safe use in file paths
     * Replaces problematic characters with safe alternatives
     * @param input The string to sanitize
     * @return Sanitized string safe for use in file paths
     */
    static std::string sanitizeForFilename(const std::string& input);
    
    /**
     * Trims whitespace from both ends of a string
     * @param input The string to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& input);
    
    /**
     * Converts string to lowercase
     * @param input The string to convert
     * @return Lowercase string
     */
    static std::string toLowerCase(const std::string& input);
    
    /**
     * Checks if a string is empty or contains only whitespace
     * @param input The string to check
     * @return True if string is empty or whitespace-only
     */
    static bool isEmptyOrWhitespace(const std::string& input);
};

#endif // STRING_UTILS_H
