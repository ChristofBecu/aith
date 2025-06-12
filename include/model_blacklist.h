#pragma once

#include <string>
#include <vector>

/**
 * @brief Structure representing a blacklisted model entry.
 */
struct BlacklistEntry {
    std::string provider;   ///< The provider name (e.g., "groq", "openai")
    std::string model;      ///< The model name (e.g., "whisper-large-v3")
    std::string reason;     ///< Optional reason for blacklisting
    std::string timestamp;  ///< When the entry was added
};

/**
 * @brief Model blacklist management for AITH application.
 * 
 * This class manages the blacklist of AI models that should not be used.
 * It handles reading from and writing to the blacklist file (~/.config/aith/blacklist)
 * with a pipe-separated format: provider | model | reason # Added on timestamp
 */
class ModelBlacklist {
public:
    /**
     * Checks if a model is blacklisted for a specific provider.
     * @param provider The provider name
     * @param modelName The name of the model to check
     * @return True if the model is blacklisted for the provider, false otherwise
     */
    static bool isModelBlacklisted(const std::string &provider, const std::string &modelName);
    
    /**
     * Adds a model to the blacklist for a specific provider.
     * @param provider The provider name
     * @param modelName The name of the model to blacklist
     * @param reason Optional reason why the model was blacklisted
     */
    static void addModelToBlacklist(const std::string &provider, const std::string &modelName, 
                                   const std::string &reason = "");
    
    /**
     * Removes a model from the blacklist for a specific provider.
     * @param provider The provider name
     * @param modelName The name of the model to remove from the blacklist
     */
    static void removeModelFromBlacklist(const std::string &provider, const std::string &modelName);
    
    /**
     * Returns a list of all blacklisted models and their information.
     * @return A vector of BlacklistEntry structures
     */
    static std::vector<BlacklistEntry> getBlacklistedModels();

private:
    /**
     * Gets the path to the blacklist file (~/.config/aith/blacklist).
     * @return The absolute path to the blacklist file
     */
    static std::string getBlacklistPath();
    
    /**
     * Parses a blacklist line into provider, model, and reason components.
     * @param line The line to parse (pipe-separated format)
     * @return Vector containing [provider, model, reason] (reason may be empty)
     */
    static std::vector<std::string> parseBlacklistLine(const std::string &line);
    
    /**
     * Trims whitespace from the beginning and end of a string.
     * @param str The string to trim
     * @return The trimmed string
     */
    static std::string trimWhitespace(const std::string &str);
    
    /**
     * Extracts timestamp from a comment line.
     * @param commentPart The comment portion of a blacklist line
     * @return The extracted timestamp or empty string if not found
     */
    static std::string extractTimestamp(const std::string &commentPart);
    
    /**
     * Generates a current timestamp string.
     * @return Current date and time as a string
     */
    static std::string getCurrentTimestamp();
    
    /**
     * Ensures the config directory exists and creates it if necessary.
     */
    static void ensureConfigDirectoryExists();
};
