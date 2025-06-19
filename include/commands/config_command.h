#pragma once

#include "command.h"
#include "application_setup.h"

/**
 * @class ConfigCommand
 * @brief Command for managing application configuration
 * 
 * This command handles all configuration management operations including:
 * - Interactive setup wizard
 * - Provider configuration CRUD operations
 * - Configuration validation and testing
 * - Backup and restore functionality
 * 
 * Delegates complex configuration logic to specialized classes in the config domain.
 */
class ConfigCommand : public Command {
public:
    /**
     * @brief Constructor for ConfigCommand
     * @param args Command line arguments
     * @param config Application configuration
     */
    ConfigCommand(const std::vector<std::string>& args, const ApplicationSetup::Config& config);
    
    /**
     * @brief Execute the config command
     * 
     * This method processes the sub-command (wizard, list, show, etc.) and executes
     * the appropriate configuration operation.
     */
    void execute() override;
    
    /**
     * @brief Validates the command arguments
     * 
     * Ensures that the config command has valid sub-commands and arguments.
     */
    void validateArgs() const override;
    
    /**
     * @brief Gets the command name for identification
     * 
     * @return The command name ("config")
     */
    std::string getCommandName() const override;

private:
    // Phase 1: Core wizard and basic operations
    /**
     * @brief Execute the interactive configuration wizard
     */
    void executeWizardCommand();
    
    /**
     * @brief List all provider configurations
     */
    void executeListCommand();
    
    /**
     * @brief Show current configuration details
     */
    void executeShowCommand();
    
    /**
     * @brief Create new provider configuration
     */
    void executeCreateCommand();
    
    /**
     * @brief Edit existing provider configuration
     */
    void executeEditCommand();
    
    /**
     * @brief Remove provider configuration
     */
    void executeRemoveCommand();
    
    // Phase 2: CRUD and Advanced operations
    /**
     * @brief Add new provider configuration (non-interactive)
     */
    void executeAddCommand();
    
    /**
     * @brief Validate configuration files
     */
    void executeValidateCommand();
    
    /**
     * @brief Set default provider
     */
    void executeSetDefaultCommand();
    
    /**
     * @brief Backup configuration files
     */
    void executeBackupCommand();
    
    /**
     * @brief Restore configuration from backup
     */
    void executeRestoreCommand();
    
    // Phase 3: Value management (to be implemented later)
    void executeGetCommand();
    void executeSetCommand();
    void executeUnsetCommand();
    
    // Helper methods
    /**
     * @brief Gets the configuration sub-command from arguments
     * 
     * @return The sub-command string (e.g., "wizard", "list", "show")
     */
    std::string getSubCommand() const;
    
    /**
     * @brief Validates arguments for specific sub-commands
     */
    void validateSubcommandArgs() const;
    
    /**
     * @brief Validates main configuration file
     * @param configPath Path to the main config file
     * @return True if valid, false otherwise
     */
    bool validateMainConfig(const std::string& configPath);
    
    /**
     * @brief Validates provider configuration file
     * @param provider Provider name
     * @param configPath Path to the provider config file
     * @return True if valid, false otherwise
     */
    bool validateProviderConfig(const std::string& provider, const std::string& configPath);
    
    std::vector<std::string> args_;
    ApplicationSetup::Config config_;
};
