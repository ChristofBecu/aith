#pragma once

#include <string>

/**
 * @brief Provider/Agent management for AI application.
 * 
 * This class manages AI providers (agents) and their configurations.
 * It handles provider selection with priority order, API key management,
 * and provider-specific configuration retrieval.
 */
class ProviderManager {
public:
    /**
     * Sets the provider from command line arguments.
     * This takes highest priority in provider resolution.
     * @param provider The provider specified via command line
     */
    static void setCommandLineProvider(const std::string &provider);
    
    /**
     * Gets the current agent/provider type to use.
     * Priority order:
     * 1. Command line specified provider
     * 2. AGENT environment variable
     * 3. Default provider from config file
     * 4. Fallback to "groq"
     * @return The agent/provider type
     */
    static std::string getAgent();
    
    /**
     * Gets the default provider from the main config file.
     * @return The default provider name (defaults to "groq" if not specified)
     */
    static std::string getDefaultProvider();
    
    /**
     * Gets the default model for the current provider.
     * Checks provider-specific config first, then falls back to main config.
     * @return The default model name
     */
    static std::string getDefaultModel();
    
    /**
     * Gets the API URL for the current provider.
     * Checks provider-specific config first, then falls back to main config.
     * @return The API URL
     */
    static std::string getApiUrl();
    
    /**
     * Gets the API key for the current provider.
     * Checks for the API key in the following order:
     * 1. Environment variable with provider prefix (e.g., GROQ_API_KEY)
     * 2. Provider-specific config file (~/.config/aith/provider.conf)
     * 3. Generic API_KEY from main config file
     * 4. For backward compatibility: GROQ_API_KEY for groq provider
     * @return The API key or empty string if not found
     */
    static std::string getApiKey();

private:
    /**
     * Static variable to store the provider specified via command line.
     * This takes highest priority in provider resolution.
     */
    static std::string commandLineProvider;
};
