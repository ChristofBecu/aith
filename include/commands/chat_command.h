#ifndef CHAT_COMMAND_H
#define CHAT_COMMAND_H

#include "command.h"
#include "application_setup.h"

/**
 * @class ChatCommand
 * @brief Command for handling chat interactions and new conversations
 * 
 * This command handles both direct chat prompts and new conversation creation.
 * It processes the user input and either starts a new conversation or continues 
 * with the current history.
 */
class ChatCommand : public Command {
public:
    /**
     * @brief Constructor for ChatCommand
     * @param args Command line arguments
     * @param config Application configuration
     */
    ChatCommand(const std::vector<std::string>& args, const ApplicationSetup::Config& config);
    
    /**
     * @brief Execute the chat command
     * 
     * This method processes the arguments to determine whether to start a new
     * conversation or continue with the current history, then initiates the chat.
     */
    void execute() override;
    
    /**
     * @brief Validates the command arguments
     * 
     * Ensures that the arguments are valid for either direct chat or new conversation.
     */
    void validateArgs() const override;
    
    /**
     * @brief Gets the command name for identification
     * 
     * @return The command name ("chat" for direct prompts, "new" for new conversations)
     */
    std::string getCommandName() const override;

private:
    std::vector<std::string> args_;
    ApplicationSetup::Config config_;
    
    /**
     * @brief Execute a direct chat command
     * @param prompt The user's prompt
     * @param model Optional model specification
     */
    void executeDirectChat(const std::string& prompt, const std::string& model = "");
    
    /**
     * @brief Execute a new conversation command
     * @param prompt The initial prompt for the new conversation
     * @param model Optional model specification
     */
    void executeNewChat(const std::string& prompt, const std::string& model = "");
};

#endif // CHAT_COMMAND_H
