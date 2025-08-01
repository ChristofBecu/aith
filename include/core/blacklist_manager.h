#pragma once

#include <string>
#include <vector>
#include <memory>

// Forward declarations
class BlacklistFileManager;
class BlacklistParser;

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
 * @brief Blacklist management service for AITH application.
 * 
 * This class manages the blacklist of AI models that should not be used.
 * It provides a high-level interface for blacklist operations, delegating
 * to specialized operation classes through the factory pattern.
 */
class BlacklistManager {
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
};
