#pragma once

#include <vector>
#include <string>
#include <memory>

/**
 * @brief Abstract base class for all application commands.
 * 
 * This class defines the common interface that all commands must implement.
 * It follows the Command pattern, allowing for clean separation of command
 * logic from the main application flow.
 * 
 * Each command is responsible for:
 * - Validating its own arguments
 * - Executing its specific functionality
 * - Handling its own error cases
 */
class Command {
public:
    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~Command() = default;

    /**
     * @brief Executes the command's main functionality.
     * 
     * This method contains the core logic of the command. It should
     * perform all necessary operations and handle any command-specific
     * errors appropriately.
     * 
     * @throws std::runtime_error for command execution failures
     */
    virtual void execute() = 0;

    /**
     * @brief Validates the command's arguments and configuration.
     * 
     * This method should check that all required arguments are present
     * and valid before execution. It should throw descriptive exceptions
     * for any validation failures.
     * 
     * @throws std::invalid_argument for invalid or missing arguments
     */
    virtual void validateArgs() const = 0;

    /**
     * @brief Gets the command name for identification purposes.
     * 
     * @return The name of this command (e.g., "list", "history", "chat")
     */
    virtual std::string getCommandName() const = 0;

protected:
    /**
     * @brief Command-line arguments passed to this command.
     * 
     * This vector contains the arguments specific to this command,
     * excluding the command name itself.
     */
    std::vector<std::string> args;

    /**
     * @brief API key for accessing AI services.
     * 
     * This is provided by the ApplicationSetup configuration
     * and is needed by most commands that interact with AI providers.
     */
    std::string apiKey;

    /**
     * @brief Current provider name (e.g., "groq", "openrouter").
     * 
     * Used for provider-specific operations and error reporting.
     */
    std::string provider;

    /**
     * @brief Path to the current history file.
     * 
     * Used by commands that need to read from or write to
     * the conversation history.
     */
    std::string currentHistory;

    /**
     * @brief Path to the history directory.
     * 
     * Used by commands that need to manage multiple history files
     * or create new history files.
     */
    std::string historyDir;

    /**
     * @brief Protected constructor to prevent direct instantiation.
     * 
     * Commands should only be created through the CommandFactory
     * to ensure proper initialization of all required fields.
     * 
     * @param commandArgs Arguments specific to this command
     * @param commandApiKey API key for AI services
     * @param commandProvider Current AI provider name
     * @param commandCurrentHistory Path to current history file
     * @param commandHistoryDir Path to history directory
     */
    Command(const std::vector<std::string>& commandArgs,
            const std::string& commandApiKey,
            const std::string& commandProvider,
            const std::string& commandCurrentHistory,
            const std::string& commandHistoryDir)
        : args(commandArgs)
        , apiKey(commandApiKey)
        , provider(commandProvider)
        , currentHistory(commandCurrentHistory)
        , historyDir(commandHistoryDir) {}
};
