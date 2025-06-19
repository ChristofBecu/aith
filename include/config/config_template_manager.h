#pragma once

#include <string>
#include <vector>
#include <map>

/**
 * @brief Configuration template data structure
 */
struct ConfigTemplate {
    std::string apiUrl;              ///< Default API URL for the provider
    std::string defaultModel;       ///< Recommended default model
    std::vector<std::string> availableModels;  ///< List of available models
    std::string description;         ///< Human-readable provider description
    std::map<std::string, std::string> additionalSettings;  ///< Extra config options
};

/**
 * @brief Manages configuration templates for different AI providers
 * 
 * Provides templates and metadata for setting up configurations
 * for various AI providers like Groq, OpenRouter, Anthropic, etc.
 * This centralizes provider-specific knowledge and makes it easy
 * to add support for new providers.
 */
class ConfigTemplateManager {
public:
    /**
     * @brief Get configuration template for a provider
     * @param provider Provider name (e.g., "groq", "openrouter")
     * @return Configuration template
     * @throws std::invalid_argument if provider is not supported
     */
    static ConfigTemplate getProviderTemplate(const std::string& provider);
    
    /**
     * @brief Get list of all supported providers
     * @return Vector of supported provider names
     */
    static std::vector<std::string> getSupportedProviders();
    
    /**
     * @brief Check if a provider is supported
     * @param provider Provider name to check
     * @return True if provider is supported
     */
    static bool isKnownProvider(const std::string& provider);
    
    /**
     * @brief Generate configuration file content from template
     * @param provider Provider name
     * @param apiKey API key to include
     * @param model Default model to use
     * @return Configuration file content as string
     */
    static std::string generateConfigContent(const std::string& provider,
                                           const std::string& apiKey,
                                           const std::string& model);
    
    /**
     * @brief Get provider description for display
     * @param provider Provider name
     * @return Human-readable provider description
     */
    static std::string getProviderDescription(const std::string& provider);
    
    /**
     * @brief Display provider template information
     * @param provider Provider name
     */
    static void displayProviderTemplate(const std::string& provider);

private:
    /**
     * @brief Initialize built-in provider templates
     * @return Map of provider names to templates
     */
    static std::map<std::string, ConfigTemplate> initializeTemplates();
    
    /**
     * @brief Get the static template map
     * @return Reference to template map
     */
    static const std::map<std::string, ConfigTemplate>& getTemplates();
    
    /**
     * @brief Normalize provider name (lowercase)
     * @param provider Provider name to normalize
     * @return Normalized provider name
     */
    static std::string normalizeProviderName(const std::string& provider);
};
