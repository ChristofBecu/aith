#include "provider_manager.h"
#include "config_manager.h"
#include "system_utils.h"

// Static variable to store the provider specified via command line
std::string ProviderManager::commandLineProvider = "";

/**
 * Sets the provider from command line arguments.
 * This takes highest priority in provider resolution.
 */
void ProviderManager::setCommandLineProvider(const std::string &provider) {
    commandLineProvider = provider;
}

/**
 * Gets the current agent/provider type to use.
 * Priority order:
 * 1. Command line specified provider
 * 2. AGENT environment variable
 * 3. Default provider from config file
 * 4. Fallback to "groq"
 */
std::string ProviderManager::getAgent() {
    // First check if provider was specified via command line
    if (!commandLineProvider.empty()) {
        return commandLineProvider;
    }
    
    // Then check environment variable
    std::string agent = SystemUtils::getEnvVar("AGENT");
    if (!agent.empty()) {
        return agent;
    }
    
    // Then check the config file
    agent = ConfigManager::getConfigValue("AGENT");
    if (!agent.empty()) {
        return agent;
    }
    
    // Finally, fall back to the default provider
    return getDefaultProvider();
}

/**
 * Gets the default provider from the main config file.
 * @return The default provider name (defaults to "groq" if not specified)
 */
std::string ProviderManager::getDefaultProvider() {
    std::string provider = ConfigManager::getConfigValue("DEFAULT_PROVIDER");
    return provider.empty() ? "groq" : provider;
}

/**
 * Gets the default model for the current provider.
 * Checks provider-specific config first, then falls back to main config.
 */
std::string ProviderManager::getDefaultModel() {
    std::string provider = getAgent();
    std::string model = ConfigManager::getProviderConfigValue(provider, "DEFAULT_MODEL");
    if (model.empty()) {
        // Fall back to main config file
        model = ConfigManager::getConfigValue("DEFAULT_MODEL");
    }
    return model;
}

/**
 * Gets the API URL for the current provider.
 * Checks provider-specific config first, then falls back to main config.
 */
std::string ProviderManager::getApiUrl() {
    std::string provider = getAgent();
    std::string url = ConfigManager::getProviderConfigValue(provider, "API_URL");
    if (url.empty()) {
        // Fall back to main config file
        url = ConfigManager::getConfigValue("API_URL");
    }
    return url;
}

/**
 * Gets the API key for the current provider.
 * Checks for the API key in the following order:
 * 1. Environment variable with provider prefix (e.g., GROQ_API_KEY)
 * 2. Provider-specific config file (~/.config/ai/provider.conf)
 * 3. Generic API_KEY from main config file
 * 4. For backward compatibility: GROQ_API_KEY for groq provider
 */
std::string ProviderManager::getApiKey() {
    std::string provider = getAgent();
    
    // Try agent-specific environment variable
    std::string envVarName = provider + "_API_KEY";
    std::string apiKey = SystemUtils::getEnvVar(envVarName);
    if (!apiKey.empty()) {
        return apiKey;
    }
    
    // For backward compatibility, try GROQ_API_KEY
    if (provider == "GROQ" || provider == "groq") {
        apiKey = SystemUtils::getEnvVar("GROQ_API_KEY");
        if (!apiKey.empty()) {
            return apiKey;
        }
    }
    
    // Read from provider-specific config file
    apiKey = ConfigManager::getProviderConfigValue(provider, "API_KEY");
    if (!apiKey.empty()) {
        return apiKey;
    }
    
    // Try generic API_KEY from main config file as last resort
    return ConfigManager::getConfigValue("API_KEY");
}
