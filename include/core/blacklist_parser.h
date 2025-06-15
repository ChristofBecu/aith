#pragma once

#include <string>
#include <vector>

/**
 * @brief Structure representing a parsed blacklist entry.
 * 
 * This structure contains all the components extracted from a blacklist line,
 * including the parsed provider, model, reason, and timestamp information.
 */
struct ParsedBlacklistEntry {
    std::string provider;   ///< The provider name (e.g., "groq", "openai")
    std::string model;      ///< The model name (e.g., "whisper-large-v3")
    std::string reason;     ///< Optional reason for blacklisting
    std::string timestamp;  ///< When the entry was added
    bool isValid;           ///< Whether the entry was successfully parsed
    std::string rawLine;    ///< The original line that was parsed
};

/**
 * @brief Handles all text processing and parsing operations for the blacklist system.
 * 
 * This class is responsible for:
 * - Parsing blacklist file lines into structured data
 * - Formatting blacklist entries for file storage
 * - String processing utilities (trimming, validation)
 * - Timestamp operations (generation, extraction)
 * - Line classification (empty, comment, valid entry)
 * 
 * The blacklist file format is: provider | model | reason # Added on timestamp
 */
class BlacklistParser {
public:
    /**
     * @brief Parses a single blacklist line into its components
     * @param line The line to parse (pipe-separated format)
     * @return ParsedBlacklistEntry containing all extracted components
     */
    static ParsedBlacklistEntry parseLine(const std::string& line);
    
    /**
     * @brief Formats a blacklist entry for storage in the file
     * @param provider The provider name
     * @param model The model name
     * @param reason Optional reason for blacklisting
     * @param timestamp When the entry was added
     * @return Formatted string ready for file storage
     */
    static std::string formatEntry(const std::string& provider, 
                                 const std::string& model,
                                 const std::string& reason = "",
                                 const std::string& timestamp = "");
                                 
    /**
     * @brief Splits a line by pipe delimiter and trims each part
     * @param line The line to split
     * @return Vector of trimmed string parts
     */
    static std::vector<std::string> splitByPipeDelimiter(const std::string& line);
    
    /**
     * @brief Trims whitespace from the beginning and end of a string
     * @param str The string to trim
     * @return The trimmed string
     */
    static std::string trimWhitespace(const std::string& str);
    
    /**
     * @brief Extracts timestamp from a comment line
     * @param commentPart The comment portion of a blacklist line
     * @return The extracted timestamp or empty string if not found
     */
    static std::string extractTimestamp(const std::string& commentPart);
    
    /**
     * @brief Generates a current timestamp string
     * @return Current date and time as a string
     */
    static std::string getCurrentTimestamp();
    
    /**
     * @brief Checks if a line is a comment line (starts with #)
     * @param line The line to check
     * @return True if the line is a comment, false otherwise
     */
    static bool isCommentLine(const std::string& line);
    
    /**
     * @brief Checks if a line is empty or contains only whitespace
     * @param line The line to check
     * @return True if the line is empty, false otherwise
     */
    static bool isEmptyLine(const std::string& line);
    
    /**
     * @brief Validates that a provider name is acceptable
     * @param provider The provider name to validate
     * @return True if valid, false otherwise
     */
    static bool isValidProvider(const std::string& provider);
    
    /**
     * @brief Validates that a model name is acceptable
     * @param model The model name to validate
     * @return True if valid, false otherwise
     */
    static bool isValidModel(const std::string& model);

private:
    /**
     * @brief Separates the main content from comment in a line
     * @param line The input line
     * @param mainPart Output parameter for the main content
     * @param commentPart Output parameter for the comment content
     */
    static void separateMainAndComment(const std::string& line, 
                                     std::string& mainPart, 
                                     std::string& commentPart);
};
