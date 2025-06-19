#include "config_wizard.h"
#include "config_template_manager.h"
#include "config_manager.h"
#include "provider_manager.h"
#include "file_operations.h"
#include "directory_operations.h"
#include "config_file_handler.h"
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <unistd.h>  // For isatty()
#include <stdexcept>

void ConfigWizard::runInteractiveSetup(bool forceNonInteractive) {
    // Check if we're in an interactive terminal
    if (!forceNonInteractive && !isInteractiveTerminal()) {
        std::cout << "❌ Configuration wizard requires an interactive terminal." << std::endl;
        std::cout << "   Please run this command directly in your terminal, not through a pipe." << std::endl;
        std::cout << "   For non-interactive configuration, use: aith config add" << std::endl;
        std::cout << "   To force the wizard in non-interactive mode, use: aith config wizard --force" << std::endl;
        return;
    }
    
    displayWelcome();
    
    try {
        // Step 1: Provider selection
        std::string provider = promptForProvider();
        
        // Step 2: API key input
        std::string apiKey = promptForApiKey(provider);
        
        // Step 3: Model selection
        std::string model = promptForModel(provider);
        
        // Step 4: Save configuration
        if (validateAndSaveConfiguration(provider, apiKey, model)) {
            displayCompletion(provider);
        } else {
            std::cout << "❌ Failed to save configuration. Please try again." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ Setup failed: " << e.what() << std::endl;
    }
}

void ConfigWizard::displayWelcome() {
    clearScreen();
    std::cout << "🧙‍♂️ Welcome to the AITH Configuration Wizard!" << std::endl;
    displaySeparator();
    std::cout << "This wizard will help you set up your first AI provider configuration." << std::endl;
    std::cout << "You'll need an API key from your chosen provider." << std::endl;
    std::cout << std::endl;
}

std::string ConfigWizard::promptForProvider() {
    std::cout << "📡 Step 1: Choose your AI provider" << std::endl;
    displaySeparator();
    
    auto providers = ConfigTemplateManager::getSupportedProviders();
    std::vector<std::string> options;
    
    for (const auto& provider : providers) {
        std::string description = ConfigTemplateManager::getProviderDescription(provider);
        options.push_back(provider + " - " + description);
    }
    
    int selection = displayMenuAndGetSelection("Available Providers:", options);
    std::string selectedProvider = providers[selection];
    
    std::cout << "✅ Selected: " << selectedProvider << std::endl;
    std::cout << std::endl;
    
    return selectedProvider;
}

std::string ConfigWizard::promptForApiKey(const std::string& provider) {
    std::cout << "🔑 Step 2: Enter your API key" << std::endl;
    displaySeparator();
    
    std::cout << "You'll need an API key for " << provider << "." << std::endl;
    
    if (ConfigTemplateManager::isKnownProvider(provider)) {
        auto config = ConfigTemplateManager::getProviderTemplate(provider);
        std::cout << "Visit: " << config.apiUrl << " to get your API key." << std::endl;
    }
    
    std::cout << std::endl;
    
    std::string apiKey;
    int maxAttempts = 3;
    int attempts = 0;
    
    while (apiKey.empty() && attempts < maxAttempts) {
        try {
            apiKey = getUserInput("Enter your API key");
            if (apiKey.empty()) {
                attempts++;
                if (attempts < maxAttempts) {
                    std::cout << "⚠️  API key cannot be empty. Please try again." << std::endl;
                } else {
                    throw std::runtime_error("API key is required - too many empty attempts");
                }
            }
        } catch (const std::runtime_error& e) {
            throw std::runtime_error("Failed to get API key: " + std::string(e.what()));
        }
    }
    
    if (apiKey.empty()) {
        throw std::runtime_error("API key is required but was not provided");
    }
    
    std::cout << "✅ API key entered" << std::endl;
    std::cout << std::endl;
    
    return apiKey;
}

std::string ConfigWizard::promptForModel(const std::string& provider) {
    std::cout << "🤖 Step 3: Choose your default model" << std::endl;
    displaySeparator();
    
    if (!ConfigTemplateManager::isKnownProvider(provider)) {
        try {
            std::string model = getUserInput("Enter default model name (or press Enter to skip)");
            return model;
        } catch (const std::runtime_error& e) {
            throw std::runtime_error("Failed to get model selection: " + std::string(e.what()));
        }
    }
    
    auto config = ConfigTemplateManager::getProviderTemplate(provider);
    
    std::cout << "Recommended model: " << config.defaultModel << std::endl;
    std::cout << std::endl;
    
    std::vector<std::string> options;
    options.push_back("Use recommended (" + config.defaultModel + ")");
    
    if (!config.availableModels.empty()) {
        for (const auto& model : config.availableModels) {
            if (model != config.defaultModel) {
                options.push_back(model);
            }
        }
    }
    
    options.push_back("Enter custom model name");
    
    try {
        int selection = displayMenuAndGetSelection("Model Options:", options);
        
        if (selection == 0) {
            std::cout << "✅ Using recommended model: " << config.defaultModel << std::endl;
            return config.defaultModel;
        } else if (selection == static_cast<int>(options.size()) - 1) {
            std::string customModel = getUserInput("Enter custom model name");
            std::cout << "✅ Using custom model: " << customModel << std::endl;
            return customModel;
        } else {
            std::string selectedModel = config.availableModels[selection - 1];
            std::cout << "✅ Selected model: " << selectedModel << std::endl;
            return selectedModel;
        }
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Failed to get model selection: " + std::string(e.what()));
    }
}

bool ConfigWizard::validateAndSaveConfiguration(const std::string& provider, 
                                               const std::string& apiKey, 
                                               const std::string& model) {
    std::cout << "💾 Step 4: Saving configuration" << std::endl;
    displaySeparator();
    
    try {
        std::string configDir = ConfigManager::getConfigDir();
        
        // Create config directory if it doesn't exist
        DirectoryOperations::create(configDir);
        
        // Generate provider config content
        std::string configContent = ConfigTemplateManager::generateConfigContent(provider, apiKey, model);
        
        // Write provider config file
        std::string providerConfigPath = configDir + "/" + provider + ".conf";
        FileOperations::write(providerConfigPath, configContent);
        
        // Update main config to set this as default provider
        std::string mainConfigPath = configDir + "/config";
        ConfigFileHandler::writeValue(mainConfigPath, "DEFAULT_PROVIDER", provider);
        
        std::cout << "✅ Configuration saved successfully!" << std::endl;
        std::cout << "   Provider config: " << providerConfigPath << std::endl;
        std::cout << "   Main config: " << mainConfigPath << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error saving configuration: " << e.what() << std::endl;
        return false;
    }
}

void ConfigWizard::displayCompletion(const std::string& provider) {
    std::cout << std::endl;
    displaySeparator();
    std::cout << "🎉 Setup Complete!" << std::endl;
    displaySeparator();
    std::cout << "Your AITH configuration is ready to use." << std::endl;
    std::cout << std::endl;
    std::cout << "Next steps:" << std::endl;
    std::cout << "• Try: aith new \"Hello, world!\"" << std::endl;
    std::cout << "• List models: aith list" << std::endl;
    std::cout << "• View config: aith config show" << std::endl;
    std::cout << "• Get help: aith help" << std::endl;
    std::cout << std::endl;
    std::cout << "Happy chatting! 🤖" << std::endl;
}

std::string ConfigWizard::getUserInput(const std::string& prompt, const std::string& defaultValue) {
    std::cout << prompt;
    if (!defaultValue.empty()) {
        std::cout << " [" << defaultValue << "]";
    }
    std::cout << ": ";
    std::cout.flush();  // Ensure prompt is displayed immediately
    
    std::string input;
    if (!safeGetline(input)) {
        throw std::runtime_error("Failed to read input (EOF reached)");
    }
    
    if (input.empty() && !defaultValue.empty()) {
        return defaultValue;
    }
    
    return input;
}

int ConfigWizard::displayMenuAndGetSelection(const std::string& title, 
                                           const std::vector<std::string>& options) {
    std::cout << title << std::endl;
    std::cout << std::endl;
    
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << options[i] << std::endl;
    }
    
    std::cout << std::endl;
    
    int selection = -1;
    int maxAttempts = 3;
    int attempts = 0;
    
    while (selection < 1 || selection > static_cast<int>(options.size())) {
        if (attempts >= maxAttempts) {
            throw std::runtime_error("Too many invalid input attempts");
        }
        
        std::cout << "Enter your choice (1-" << options.size() << "): ";
        std::cout.flush();
        
        std::string input;
        if (!safeGetline(input)) {
            throw std::runtime_error("Failed to read input (EOF reached)");
        }
        
        attempts++;
        
        try {
            selection = std::stoi(input);
            if (selection < 1 || selection > static_cast<int>(options.size())) {
                std::cout << "⚠️  Invalid choice. Please enter a number between 1 and " 
                         << options.size() << "." << std::endl;
            }
        } catch (const std::exception&) {
            std::cout << "⚠️  Invalid input. Please enter a number." << std::endl;
        }
    }
    
    return selection - 1;  // Convert to 0-based index
}

void ConfigWizard::clearScreen() {
    // Clear screen (works on most terminals)
    std::cout << "\033[2J\033[H";
}

void ConfigWizard::displaySeparator() {
    std::cout << "─────────────────────────────────────────────────────────" << std::endl;
}

bool ConfigWizard::isInteractiveTerminal() {
    return isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
}

bool ConfigWizard::safeGetline(std::string& input) {
    if (std::cin.eof()) {
        return false;
    }
    
    if (!std::getline(std::cin, input)) {
        if (std::cin.eof()) {
            return false;
        }
        // Clear error state if not EOF
        std::cin.clear();
        return false;
    }
    
    return true;
}
