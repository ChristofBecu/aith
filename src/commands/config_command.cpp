#include "config_command.h"
#include "config_wizard.h"
#include "config_template_manager.h"
#include "config_manager.h"
#include "provider_manager.h"
#include "system_utils.h"
#include "file_operations.h"
#include "directory_operations.h"
#include "config_file_handler.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <ctime>

ConfigCommand::ConfigCommand(const std::vector<std::string>& args, const ApplicationSetup::Config& config)
    : Command(args, config.apiKey, config.provider, config.currentHistory, config.historyDir),
      args_(args), config_(config) {
}

void ConfigCommand::execute() {
    validateArgs();
    
    std::string subCommand = getSubCommand();
    
    if (subCommand == "wizard") {
        executeWizardCommand();
    } else if (subCommand == "list") {
        executeListCommand();
    } else if (subCommand == "show") {
        executeShowCommand();
    } else if (subCommand == "add") {
        executeAddCommand();
    } else if (subCommand == "create") {
        executeCreateCommand();
    } else if (subCommand == "edit") {
        executeEditCommand();
    } else if (subCommand == "remove" || subCommand == "delete") {
        executeRemoveCommand();
    } else if (subCommand == "validate") {
        executeValidateCommand();
    } else if (subCommand == "set-default") {
        executeSetDefaultCommand();
    } else if (subCommand == "backup") {
        executeBackupCommand();
    } else if (subCommand == "restore") {
        executeRestoreCommand();
    } else {
        throw std::runtime_error("Unknown config command: " + subCommand + 
                                ". Available commands: wizard, list, show, add, create, edit, remove, validate, set-default, backup, restore");
    }
}

void ConfigCommand::validateArgs() const {
    if (args_.size() < 2) {
        throw std::invalid_argument("Usage: aith config [wizard|list|show|add|create|edit|remove|validate|set-default|backup|restore] [options]");
    }
    
    validateSubcommandArgs();
}

std::string ConfigCommand::getCommandName() const {
    return "config";
}

void ConfigCommand::executeWizardCommand() {
    std::cout << "🧙‍♂️ Starting AITH Configuration Wizard...\n" << std::endl;
    
    // Check for --force flag
    bool forceNonInteractive = false;
    if (args_.size() > 2) {
        for (size_t i = 2; i < args_.size(); ++i) {
            if (args_[i] == "--force" || args_[i] == "-f") {
                forceNonInteractive = true;
                break;
            }
        }
    }
    
    ConfigWizard::runInteractiveSetup(forceNonInteractive);
}

void ConfigCommand::executeListCommand() {
    std::cout << "📋 Configuration Files:\n" << std::endl;
    
    std::string configDir = ConfigManager::getConfigDir();
    
    if (!DirectoryOperations::exists(configDir)) {
        std::cout << "No configuration directory found at: " << configDir << std::endl;
        std::cout << "Run 'aith config wizard' to set up your first configuration." << std::endl;
        return;
    }
    
    auto files = DirectoryOperations::list(configDir);
    bool foundConfigs = false;
    
    // Show main config
    std::string mainConfigPath = configDir + "/config";
    if (FileOperations::exists(mainConfigPath)) {
        std::cout << "📄 Main Configuration: " << mainConfigPath << std::endl;
        foundConfigs = true;
    }
    
    // Show provider configs
    for (const auto& file : files) {
        if (file.size() > 5 && file.substr(file.size() - 5) == ".conf") {
            std::string providerName = file.substr(0, file.size() - 5);
            std::cout << "⚙️  Provider '" << providerName << "': " << configDir << "/" << file << std::endl;
            foundConfigs = true;
        }
    }
    
    if (!foundConfigs) {
        std::cout << "No configuration files found." << std::endl;
        std::cout << "Run 'aith config wizard' to set up your first configuration." << std::endl;
    } else {
        std::cout << "\nCurrent provider: " << config_.provider << std::endl;
    }
}

void ConfigCommand::executeShowCommand() {
    std::string target = "current";
    if (args_.size() > 2) {
        target = args_[2];
    }
    
    std::cout << "🔍 Configuration Details:\n" << std::endl;
    
    if (target == "current" || target == config_.provider) {
        std::cout << "Current Provider: " << config_.provider << std::endl;
        std::cout << "API Key: " << (config_.apiKey.empty() ? "❌ Not configured" : "✅ Configured") << std::endl;
        
        // Show more details from the provider config
        std::string apiUrl = ProviderManager::getApiUrl();
        std::string defaultModel = ProviderManager::getDefaultModel();
        
        if (!apiUrl.empty()) {
            std::cout << "API URL: " << apiUrl << std::endl;
        }
        if (!defaultModel.empty()) {
            std::cout << "Default Model: " << defaultModel << std::endl;
        }
    } else {
        // Show specific provider configuration
        std::string configDir = ConfigManager::getConfigDir();
        std::string configPath = configDir + "/" + target + ".conf";
        
        if (FileOperations::exists(configPath)) {
            std::cout << "Provider: " << target << std::endl;
            std::cout << "Config File: " << configPath << std::endl;
            
            std::string apiKey = ConfigManager::getProviderConfigValue(target, "API_KEY");
            std::string apiUrl = ConfigManager::getProviderConfigValue(target, "API_URL");
            std::string model = ConfigManager::getProviderConfigValue(target, "DEFAULT_MODEL");
            
            std::cout << "API Key: " << (apiKey.empty() ? "❌ Not configured" : "✅ Configured") << std::endl;
            if (!apiUrl.empty()) {
                std::cout << "API URL: " << apiUrl << std::endl;
            }
            if (!model.empty()) {
                std::cout << "Default Model: " << model << std::endl;
            }
        } else {
            std::cout << "Configuration not found for provider: " << target << std::endl;
        }
    }
}

void ConfigCommand::executeAddCommand() {
    if (args_.size() < 4) {
        throw std::invalid_argument("Usage: aith config add <provider> <api-key> [model]");
    }
    
    std::string provider = args_[2];
    std::string apiKey = args_[3];
    std::string model = (args_.size() > 4) ? args_[4] : "";
    
    std::cout << "➕ Adding configuration for provider: " << provider << std::endl;
    
    // Check if provider is known
    if (!ConfigTemplateManager::isKnownProvider(provider)) {
        std::cout << "⚠️  Unknown provider: " << provider << std::endl;
        auto supported = ConfigTemplateManager::getSupportedProviders();
        std::cout << "Supported providers: ";
        for (size_t i = 0; i < supported.size(); ++i) {
            std::cout << supported[i];
            if (i < supported.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
        std::cout << "Proceeding with basic configuration..." << std::endl;
    }
    
    std::string configDir = ConfigManager::getConfigDir();
    std::string configPath = configDir + "/" + provider + ".conf";
    
    // Check if config already exists
    if (FileOperations::exists(configPath)) {
        std::cout << "⚠️  Configuration already exists for provider: " << provider << std::endl;
        
        // Ask for confirmation to overwrite
        std::cout << "Overwrite existing configuration? (y/N): ";
        std::string response;
        std::getline(std::cin, response);
        
        if (response != "y" && response != "Y" && response != "yes") {
            std::cout << "Cancelled. Use 'aith config edit " << provider << "' to modify existing configuration." << std::endl;
            return;
        }
        
        // Backup existing config
        std::string backupPath = configPath + ".backup." + std::to_string(time(nullptr));
        FileOperations::copy(configPath, backupPath);
        std::cout << "📋 Backed up existing config to: " << backupPath << std::endl;
    }
    
    // Create directory if it doesn't exist
    DirectoryOperations::create(configDir);
    
    // Generate config content
    std::string content = ConfigTemplateManager::generateConfigContent(provider, apiKey, model);
    
    // Write config file
    FileOperations::write(configPath, content);
    
    std::cout << "✅ Added configuration for provider: " << provider << std::endl;
    std::cout << "   Config file: " << configPath << std::endl;
    
    if (ConfigTemplateManager::isKnownProvider(provider)) {
        auto config = ConfigTemplateManager::getProviderTemplate(provider);
        std::cout << "   API URL: " << config.apiUrl << std::endl;
        std::cout << "   Default model: " << (model.empty() ? config.defaultModel : model) << std::endl;
    }
    
    // Ask if this should be the default provider
    std::cout << "\nSet " << provider << " as the default provider? (y/N): ";
    std::string response;
    std::getline(std::cin, response);
    
    if (response == "y" || response == "Y" || response == "yes") {
        std::string mainConfigPath = configDir + "/config";
        ConfigFileHandler::writeValue(mainConfigPath, "DEFAULT_PROVIDER", provider);
        std::cout << "✅ Set " << provider << " as the default provider." << std::endl;
    }
}
void ConfigCommand::executeCreateCommand() {
    if (args_.size() < 3) {
        throw std::invalid_argument("Usage: aith config create <provider>");
    }
    
    std::string provider = args_[2];
    
    std::cout << "📄 Creating configuration template for provider: " << provider << std::endl;
    
    // Check if provider is known
    if (!ConfigTemplateManager::isKnownProvider(provider)) {
        std::cout << "⚠️  Unknown provider: " << provider << std::endl;
        std::cout << "Supported providers: ";
        auto supported = ConfigTemplateManager::getSupportedProviders();
        for (size_t i = 0; i < supported.size(); ++i) {
            std::cout << supported[i];
            if (i < supported.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
        std::cout << "Creating basic template anyway..." << std::endl;
    }
    
    std::string configDir = ConfigManager::getConfigDir();
    std::string configPath = configDir + "/" + provider + ".conf";
    
    // Check if config already exists
    if (FileOperations::exists(configPath)) {
        std::cout << "⚠️  Configuration already exists for provider: " << provider << std::endl;
        std::cout << "Use 'aith config edit " << provider << "' to modify it." << std::endl;
        std::cout << "Or use 'aith config add " << provider << " <api-key>' to overwrite." << std::endl;
        return;
    }
    
    // Create directory if it doesn't exist
    DirectoryOperations::create(configDir);
    
    // Generate config content
    std::string content;
    if (ConfigTemplateManager::isKnownProvider(provider)) {
        content = ConfigTemplateManager::generateConfigContent(provider, "", "");
    } else {
        // Basic template for unknown providers
        content = "# Configuration for " + provider + " provider\n";
        content += "# Generated by AITH Configuration Management\n\n";
        content += "# API endpoint URL\n";
        content += "API_URL=\n\n";
        content += "# Your API key\n";
        content += "API_KEY=\n\n";
        content += "# Default model to use\n";
        content += "DEFAULT_MODEL=\n";
    }
    
    // Write config file
    FileOperations::write(configPath, content);
    
    std::cout << "✅ Created configuration template: " << configPath << std::endl;
    std::cout << "📝 Edit the file to add your API key and configure settings." << std::endl;
    std::cout << "   Use: aith config edit " << provider << std::endl;
    
    if (ConfigTemplateManager::isKnownProvider(provider)) {
        std::cout << "\n📋 Provider information:" << std::endl;
        ConfigTemplateManager::displayProviderTemplate(provider);
    }
}

void ConfigCommand::executeEditCommand() {
    if (args_.size() < 3) {
        throw std::invalid_argument("Usage: aith config edit <provider>");
    }
    
    std::string provider = args_[2];
    std::string configDir = ConfigManager::getConfigDir();
    std::string configPath = configDir + "/" + provider + ".conf";
    
    if (!FileOperations::exists(configPath)) {
        std::cout << "Configuration not found for provider: " << provider << std::endl;
        std::cout << "Use 'aith config create " << provider << "' to create it first." << std::endl;
        return;
    }
    
    // Get system editor
    std::string editor = SystemUtils::getEnvVar("EDITOR");
    if (editor.empty()) {
        editor = "nano";  // Default to nano if no EDITOR set
    }
    
    std::cout << "Opening " << configPath << " with " << editor << "..." << std::endl;
    
    // Launch editor
    std::string command = editor + " \"" + configPath + "\"";
    int result = system(command.c_str());
    
    if (result == 0) {
        std::cout << "✅ Configuration updated successfully." << std::endl;
    } else {
        std::cout << "⚠️  Editor exited with error code: " << result << std::endl;
    }
}

void ConfigCommand::executeRemoveCommand() {
    if (args_.size() < 3) {
        throw std::invalid_argument("Usage: aith config remove <provider>");
    }
    
    std::string provider = args_[2];
    
    std::cout << "🗑️  Removing configuration for provider: " << provider << std::endl;
    
    std::string configDir = ConfigManager::getConfigDir();
    std::string configPath = configDir + "/" + provider + ".conf";
    
    if (!FileOperations::exists(configPath)) {
        std::cout << "❌ Configuration not found for provider: " << provider << std::endl;
        return;
    }
    
    // Show what will be removed
    std::cout << "📄 Configuration file: " << configPath << std::endl;
    
    // Check if this is the default provider
    std::string mainConfigPath = configDir + "/config";
    bool isDefaultProvider = false;
    if (FileOperations::exists(mainConfigPath)) {
        try {
            std::string defaultProvider = ConfigFileHandler::readValue(mainConfigPath, "DEFAULT_PROVIDER");
            if (defaultProvider == provider) {
                isDefaultProvider = true;
                std::cout << "⚠️  This is your default provider!" << std::endl;
            }
        } catch (...) {
            // Ignore errors reading main config
        }
    }
    
    // Confirmation prompt
    std::cout << "⚠️  Are you sure you want to remove configuration for '" << provider << "'? (y/N): ";
    std::string response;
    std::getline(std::cin, response);
    
    if (response == "y" || response == "Y" || response == "yes") {
        // Create backup before removing
        std::string backupPath = configPath + ".backup." + std::to_string(time(nullptr));
        try {
            FileOperations::copy(configPath, backupPath);
            std::cout << "📋 Backed up to: " << backupPath << std::endl;
        } catch (const std::exception& e) {
            std::cout << "⚠️  Failed to create backup: " << e.what() << std::endl;
        }
        
        // Remove the configuration file
        FileOperations::remove(configPath);
        std::cout << "✅ Removed configuration for provider: " << provider << std::endl;
        
        // If this was the default provider, suggest setting a new one
        if (isDefaultProvider) {
            std::cout << "\n⚠️  You removed the default provider. Consider setting a new default:" << std::endl;
            
            auto files = DirectoryOperations::list(configDir);
            for (const auto& file : files) {
                if (file.size() > 5 && file.substr(file.size() - 5) == ".conf") {
                    std::string availableProvider = file.substr(0, file.size() - 5);
                    std::cout << "   aith config set-default " << availableProvider << std::endl;
                    break; // Show just one example
                }
            }
        }
    } else {
        std::cout << "Cancelled." << std::endl;
    }
}

void ConfigCommand::executeValidateCommand() {
    std::string target = "all";
    if (args_.size() > 2) {
        target = args_[2];
    }
    
    std::cout << "🔍 Validating configuration..." << std::endl;
    
    std::string configDir = ConfigManager::getConfigDir();
    
    if (!DirectoryOperations::exists(configDir)) {
        std::cout << "❌ Configuration directory not found: " << configDir << std::endl;
        std::cout << "Run 'aith config wizard' to set up your first configuration." << std::endl;
        return;
    }
    
    bool allValid = true;
    
    if (target == "all") {
        // Validate all configurations
        auto files = DirectoryOperations::list(configDir);
        
        // Validate main config
        std::string mainConfigPath = configDir + "/config";
        if (FileOperations::exists(mainConfigPath)) {
            std::cout << "\n📄 Validating main configuration..." << std::endl;
            if (validateMainConfig(mainConfigPath)) {
                std::cout << "✅ Main configuration is valid." << std::endl;
            } else {
                std::cout << "❌ Main configuration has issues." << std::endl;
                allValid = false;
            }
        } else {
            std::cout << "⚠️  Main configuration file not found: " << mainConfigPath << std::endl;
            allValid = false;
        }
        
        // Validate provider configs
        for (const auto& file : files) {
            if (file.size() > 5 && file.substr(file.size() - 5) == ".conf") {
                std::string provider = file.substr(0, file.size() - 5);
                std::string configPath = configDir + "/" + file;
                
                std::cout << "\n⚙️  Validating " << provider << " configuration..." << std::endl;
                if (validateProviderConfig(provider, configPath)) {
                    std::cout << "✅ " << provider << " configuration is valid." << std::endl;
                } else {
                    std::cout << "❌ " << provider << " configuration has issues." << std::endl;
                    allValid = false;
                }
            }
        }
    } else {
        // Validate specific provider
        std::string configPath = configDir + "/" + target + ".conf";
        if (FileOperations::exists(configPath)) {
            std::cout << "⚙️  Validating " << target << " configuration..." << std::endl;
            if (validateProviderConfig(target, configPath)) {
                std::cout << "✅ " << target << " configuration is valid." << std::endl;
            } else {
                std::cout << "❌ " << target << " configuration has issues." << std::endl;
                allValid = false;
            }
        } else {
            std::cout << "❌ Configuration not found for provider: " << target << std::endl;
            allValid = false;
        }
    }
    
    std::cout << "\n" << (allValid ? "🎉 All configurations are valid!" : "⚠️  Some configurations need attention.") << std::endl;
}

bool ConfigCommand::validateMainConfig(const std::string& configPath) {
    try {
        std::string defaultProvider = ConfigFileHandler::readValue(configPath, "DEFAULT_PROVIDER");
        
        if (defaultProvider.empty()) {
            std::cout << "⚠️  No default provider set in main configuration." << std::endl;
            return false;
        }
        
        // Check if the default provider config exists
        std::string configDir = ConfigManager::getConfigDir();
        std::string providerConfigPath = configDir + "/" + defaultProvider + ".conf";
        
        if (!FileOperations::exists(providerConfigPath)) {
            std::cout << "❌ Default provider '" << defaultProvider << "' config not found: " << providerConfigPath << std::endl;
            return false;
        }
        
        std::cout << "   Default provider: " << defaultProvider << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error reading main config: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigCommand::validateProviderConfig(const std::string& provider, const std::string& configPath) {
    try {
        std::string apiKey = ConfigFileHandler::readValue(configPath, "API_KEY");
        std::string apiUrl = ConfigFileHandler::readValue(configPath, "API_URL");
        std::string defaultModel = ConfigFileHandler::readValue(configPath, "DEFAULT_MODEL");
        
        bool isValid = true;
        
        if (apiKey.empty()) {
            std::cout << "❌ API key is missing or empty." << std::endl;
            isValid = false;
        } else {
            std::cout << "   API key: ✅ Present" << std::endl;
        }
        
        if (apiUrl.empty()) {
            std::cout << "❌ API URL is missing or empty." << std::endl;
            isValid = false;
        } else {
            std::cout << "   API URL: " << apiUrl << std::endl;
            
            // Basic URL validation
            if (apiUrl.find("http://") != 0 && apiUrl.find("https://") != 0) {
                std::cout << "⚠️  API URL should start with http:// or https://" << std::endl;
                isValid = false;
            }
        }
        
        if (defaultModel.empty()) {
            std::cout << "⚠️  Default model is not specified." << std::endl;
        } else {
            std::cout << "   Default model: " << defaultModel << std::endl;
            
            // Check if model is in the list of available models for known providers
            if (ConfigTemplateManager::isKnownProvider(provider)) {
                auto config = ConfigTemplateManager::getProviderTemplate(provider);
                auto& availableModels = config.availableModels;
                
                bool modelFound = std::find(availableModels.begin(), availableModels.end(), defaultModel) != availableModels.end();
                if (!modelFound && !availableModels.empty()) {
                    std::cout << "⚠️  Model '" << defaultModel << "' is not in the list of known models for " << provider << std::endl;
                    std::cout << "   Available models: ";
                    for (size_t i = 0; i < availableModels.size(); i++) {
                        std::cout << availableModels[i];
                        if (i < availableModels.size() - 1) std::cout << ", ";
                    }
                    std::cout << std::endl;
                }
            }
        }
        
        return isValid;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error reading provider config: " << e.what() << std::endl;
        return false;
    }
}

std::string ConfigCommand::getSubCommand() const {
    if (args_.size() < 2) {
        return "";
    }
    return args_[1];
}

void ConfigCommand::validateSubcommandArgs() const {
    std::string subCommand = getSubCommand();
    
    if (subCommand == "add") {
        if (args_.size() < 4) {
            throw std::invalid_argument("Usage: aith config add <provider> <api-key> [model]");
        }
    } else if (subCommand == "create" || subCommand == "edit" || subCommand == "remove" || 
               subCommand == "delete" || subCommand == "set-default") {
        if (args_.size() < 3) {
            throw std::invalid_argument("The '" + subCommand + "' command requires a provider name");
        }
    }
    // wizard, list, show, validate, backup, restore commands are more flexible with arguments
}

void ConfigCommand::executeSetDefaultCommand() {
    if (args_.size() < 3) {
        throw std::invalid_argument("Usage: aith config set-default <provider>");
    }
    
    std::string provider = args_[2];
    
    std::cout << "🔧 Setting default provider to: " << provider << std::endl;
    
    std::string configDir = ConfigManager::getConfigDir();
    std::string providerConfigPath = configDir + "/" + provider + ".conf";
    
    // Check if provider config exists
    if (!FileOperations::exists(providerConfigPath)) {
        std::cout << "❌ Configuration not found for provider: " << provider << std::endl;
        std::cout << "Available providers:" << std::endl;
        
        auto files = DirectoryOperations::list(configDir);
        bool hasProviders = false;
        for (const auto& file : files) {
            if (file.size() > 5 && file.substr(file.size() - 5) == ".conf") {
                std::string availableProvider = file.substr(0, file.size() - 5);
                std::cout << "  - " << availableProvider << std::endl;
                hasProviders = true;
            }
        }
        
        if (!hasProviders) {
            std::cout << "  (none found - run 'aith config wizard' to create one)" << std::endl;
        }
        return;
    }
    
    // Validate the provider configuration first
    std::cout << "🔍 Validating " << provider << " configuration..." << std::endl;
    if (!validateProviderConfig(provider, providerConfigPath)) {
        std::cout << "⚠️  Provider configuration has issues. Set as default anyway? (y/N): ";
        std::string response;
        std::getline(std::cin, response);
        
        if (response != "y" && response != "Y" && response != "yes") {
            std::cout << "Cancelled. Fix the configuration issues first." << std::endl;
            return;
        }
    }
    
    // Update main config
    std::string mainConfigPath = configDir + "/config";
    
    // Create main config if it doesn't exist
    DirectoryOperations::create(configDir);
    
    // Backup existing main config if it exists
    if (FileOperations::exists(mainConfigPath)) {
        std::string currentDefault = ConfigFileHandler::readValue(mainConfigPath, "DEFAULT_PROVIDER");
        if (!currentDefault.empty()) {
            std::cout << "📋 Previous default provider: " << currentDefault << std::endl;
        }
    }
    
    ConfigFileHandler::writeValue(mainConfigPath, "DEFAULT_PROVIDER", provider);
    
    std::cout << "✅ Set " << provider << " as the default provider." << std::endl;
    std::cout << "   Main config: " << mainConfigPath << std::endl;
    
    // Show provider details
    if (ConfigTemplateManager::isKnownProvider(provider)) {
        auto config = ConfigTemplateManager::getProviderTemplate(provider);
        std::cout << "   API URL: " << config.apiUrl << std::endl;
        std::cout << "   Default model: " << config.defaultModel << std::endl;
    }
}

void ConfigCommand::executeBackupCommand() {
    std::string backupName = "backup_" + std::to_string(time(nullptr));
    if (args_.size() > 2) {
        backupName = args_[2];
    }
    
    std::cout << "💾 Creating configuration backup: " << backupName << std::endl;
    
    std::string configDir = ConfigManager::getConfigDir();
    std::string backupDir = configDir + "/backups";
    std::string backupPath = backupDir + "/" + backupName;
    
    if (!DirectoryOperations::exists(configDir)) {
        std::cout << "❌ Configuration directory not found: " << configDir << std::endl;
        return;
    }
    
    // Create backup directory
    DirectoryOperations::create(backupDir);
    DirectoryOperations::create(backupPath);
    
    // Copy all configuration files
    auto files = DirectoryOperations::list(configDir);
    int filesBackedUp = 0;
    
    for (const auto& file : files) {
        if (file == "backups") continue; // Skip the backups directory itself
        
        std::string sourcePath = configDir + "/" + file;
        std::string destPath = backupPath + "/" + file;
        
        if (FileOperations::exists(sourcePath)) {
            try {
                FileOperations::copy(sourcePath, destPath);
                std::cout << "   ✅ " << file << std::endl;
                filesBackedUp++;
            } catch (const std::exception& e) {
                std::cout << "   ❌ Failed to backup " << file << ": " << e.what() << std::endl;
            }
        }
    }
    
    if (filesBackedUp > 0) {
        std::cout << "✅ Backup completed: " << backupPath << std::endl;
        std::cout << "   Files backed up: " << filesBackedUp << std::endl;
        
        // Create a backup manifest
        std::string manifestPath = backupPath + "/manifest.txt";
        std::string manifest = "AITH Configuration Backup\n";
        manifest += "Created: " + std::to_string(time(nullptr)) + "\n";
        manifest += "Files: " + std::to_string(filesBackedUp) + "\n";
        manifest += "Source: " + configDir + "\n";
        FileOperations::write(manifestPath, manifest);
    } else {
        std::cout << "⚠️  No files were backed up." << std::endl;
    }
}

void ConfigCommand::executeRestoreCommand() {
    if (args_.size() < 3) {
        std::cout << "📋 Available backups:" << std::endl;
        
        std::string configDir = ConfigManager::getConfigDir();
        std::string backupDir = configDir + "/backups";
        
        if (!DirectoryOperations::exists(backupDir)) {
            std::cout << "   (no backups found)" << std::endl;
            std::cout << "\nUsage: aith config restore <backup-name>" << std::endl;
            return;
        }
        
        auto backups = DirectoryOperations::list(backupDir);
        if (backups.empty()) {
            std::cout << "   (no backups found)" << std::endl;
        } else {
            for (const auto& backup : backups) {
                std::string backupPath = backupDir + "/" + backup;
                std::string manifestPath = backupPath + "/manifest.txt";
                
                std::cout << "   📁 " << backup;
                if (FileOperations::exists(manifestPath)) {
                    // Try to read creation time from manifest
                    try {
                        std::string manifest = FileOperations::read(manifestPath);
                        size_t createdPos = manifest.find("Created: ");
                        if (createdPos != std::string::npos) {
                            size_t timestampStart = createdPos + 9;
                            size_t timestampEnd = manifest.find("\n", timestampStart);
                            if (timestampEnd != std::string::npos) {
                                std::string timestamp = manifest.substr(timestampStart, timestampEnd - timestampStart);
                                std::cout << " (created: " << timestamp << ")";
                            }
                        }
                    } catch (...) {
                        // Ignore errors reading manifest
                    }
                }
                std::cout << std::endl;
            }
        }
        
        std::cout << "\nUsage: aith config restore <backup-name>" << std::endl;
        return;
    }
    
    std::string backupName = args_[2];
    
    std::cout << "🔄 Restoring configuration from backup: " << backupName << std::endl;
    
    std::string configDir = ConfigManager::getConfigDir();
    std::string backupDir = configDir + "/backups";
    std::string backupPath = backupDir + "/" + backupName;
    
    if (!DirectoryOperations::exists(backupPath)) {
        std::cout << "❌ Backup not found: " << backupPath << std::endl;
        return;
    }
    
    // Confirmation prompt
    std::cout << "⚠️  This will overwrite your current configuration. Continue? (y/N): ";
    std::string response;
    std::getline(std::cin, response);
    
    if (response != "y" && response != "Y" && response != "yes") {
        std::cout << "Cancelled." << std::endl;
        return;
    }
    
    // Create a backup of current config before restoring
    std::string currentBackupName = "pre_restore_" + std::to_string(time(nullptr));
    std::cout << "📋 Creating backup of current configuration: " << currentBackupName << std::endl;
    
    // Temporarily change args to create backup
    auto originalArgs = args_;
    args_ = {"config", "backup", currentBackupName};
    executeBackupCommand();
    args_ = originalArgs;
    
    // Restore files from backup
    auto files = DirectoryOperations::list(backupPath);
    int filesRestored = 0;
    
    for (const auto& file : files) {
        if (file == "manifest.txt") continue; // Skip manifest file
        
        std::string sourcePath = backupPath + "/" + file;
        std::string destPath = configDir + "/" + file;
        
        if (FileOperations::exists(sourcePath)) {
            try {
                FileOperations::copy(sourcePath, destPath);
                std::cout << "   ✅ " << file << std::endl;
                filesRestored++;
            } catch (const std::exception& e) {
                std::cout << "   ❌ Failed to restore " << file << ": " << e.what() << std::endl;
            }
        }
    }
    
    if (filesRestored > 0) {
        std::cout << "✅ Restore completed!" << std::endl;
        std::cout << "   Files restored: " << filesRestored << std::endl;
        std::cout << "   Current config backed up as: " << currentBackupName << std::endl;
    } else {
        std::cout << "⚠️  No files were restored." << std::endl;
    }
}
