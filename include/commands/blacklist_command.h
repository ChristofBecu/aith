#ifndef BLACKLIST_COMMAND_H
#define BLACKLIST_COMMAND_H

#include "command.h"
#include "application_setup.h"

/**
 * @class BlacklistCommand
 * @brief Command for managing model blacklist operations
 * 
 * This command handles blacklist management operations including:
 * - Adding models to the blacklist
 * - Removing models from the blacklist  
 * - Listing blacklisted models
 */
class BlacklistCommand : public Command {
public:
    /**
     * @brief Constructor for BlacklistCommand
     * @param args Command line arguments
     * @param config Application configuration
     */
    BlacklistCommand(const std::vector<std::string>& args, const ApplicationSetup::Config& config);
    
    /**
     * @brief Execute the blacklist command
     * 
     * This method processes the sub-command (add, remove, list) and executes
     * the appropriate blacklist operation.
     */
    void execute() override;
    
    /**
     * @brief Validates the command arguments
     * 
     * Ensures that the blacklist command has valid sub-commands and arguments.
     */
    void validateArgs() const override;
    
    /**
     * @brief Gets the command name for identification
     * 
     * @return The command name ("blacklist")
     */
    std::string getCommandName() const override;

private:
    std::vector<std::string> args_;
    ApplicationSetup::Config config_;
    
    /**
     * @brief Execute the list sub-command
     * 
     * Lists all currently blacklisted models with their details.
     */
    void executeListCommand();
    
    /**
     * @brief Execute the add sub-command
     * 
     * Adds a model to the blacklist with optional reason.
     */
    void executeAddCommand();
    
    /**
     * @brief Execute the remove sub-command
     * 
     * Removes a model from the blacklist.
     */
    void executeRemoveCommand();
    
    /**
     * @brief Gets the blacklist sub-command from arguments
     * 
     * @return The sub-command string ("add", "remove", "list")
     */
    std::string getSubCommand() const;
    
    /**
     * @brief Validates arguments for the add sub-command
     */
    void validateAddCommandArgs() const;
    
    /**
     * @brief Validates arguments for the remove sub-command
     */
    void validateRemoveCommandArgs() const;
    
    /**
     * @brief Constructs a reason string from multiple arguments
     * 
     * @param startIndex The index to start collecting reason arguments from
     * @return The combined reason string
     */
    std::string buildReasonString(size_t startIndex) const;
};

#endif // BLACKLIST_COMMAND_H
