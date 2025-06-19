#include "config_command.h"
#include "config_wizard.h"
#include "config_template_manager.h"
#include "config_manager.h"
#include "provider_manager.h"
#include "system_utils.h"
#include "file_operations.h"
#include "directory_operations.h"
#include <iostream>
#include <stdexcept>

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
    } else if (subCommand == "create") {
        executeCreateCommand();
    } else if (subCommand == "edit") {
        executeEditCommand();
    } else if (subCommand == "remove") {
        executeRemoveCommand();
    } else {
        throw std::runtime_error("Unknown config command: " + subCommand + 
                                ". Available commands: wizard, list, show, create, edit, remove");
    }
}

void ConfigCommand::validateArgs() const {
    if (args_.size() < 2) {
        throw std::invalid_argument("Usage: aith config [wizard|list|show|create|edit|remove] [options]");
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

void ConfigCommand::executeCreateCommand() {
    if (args_.size() < 3) {
        throw std::invalid_argument("Usage: aith config create <provider>");
    }
    
    std::string provider = args_[2];
    
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
        std::cout << "Configuration already exists for provider: " << provider << std::endl;
        std::cout << "Use 'aith config edit " << provider << "' to modify it." << std::endl;
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
        content = "# Configuration for " + provider + "\n";
        content += "API_URL=\n";
        content += "API_KEY=\n";
        content += "DEFAULT_MODEL=\n";
    }
    
    // Write config file
    FileOperations::write(configPath, content);
    
    std::cout << "✅ Created configuration file: " << configPath << std::endl;
    std::cout << "Edit the file to add your API key and configure settings." << std::endl;
    
    if (ConfigTemplateManager::isKnownProvider(provider)) {
        std::cout << "\nProvider information:" << std::endl;
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
    std::string configDir = ConfigManager::getConfigDir();
    std::string configPath = configDir + "/" + provider + ".conf";
    
    if (!FileOperations::exists(configPath)) {
        std::cout << "Configuration not found for provider: " << provider << std::endl;
        return;
    }
    
    // Confirmation prompt
    std::cout << "⚠️  Are you sure you want to remove configuration for '" << provider << "'? (y/N): ";
    std::string response;
    std::getline(std::cin, response);
    
    if (response == "y" || response == "Y" || response == "yes") {
        FileOperations::remove(configPath);
        std::cout << "✅ Removed configuration for provider: " << provider << std::endl;
    } else {
        std::cout << "Cancelled." << std::endl;
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
    
    if (subCommand == "create" || subCommand == "edit" || subCommand == "remove") {
        if (args_.size() < 3) {
            throw std::invalid_argument("The '" + subCommand + "' command requires a provider name");
        }
    }
    // wizard, list, show commands are more flexible with arguments
}
