#pragma once

#include "command.h"

/**
 * @brief Command to display conversation history files and view conversation content.
 * 
 * This command supports multiple subcommands for managing conversation history:
 * 
 * Usage:
 *   ./aith history              - List all history files and show current file
 *   ./aith history view [target] - View conversation content in formatted display
 * 
 * For the 'view' subcommand, target can be:
 * - "current" or "." - View the current active history file
 * - "latest" - View the most recently modified history file
 * - filename - View a specific history file (e.g., "conversation_20240101_120000.json")
 * 
 * The command will:
 * - List all files in the history directory (default behavior)
 * - Display conversation content in a user-friendly format (view subcommand)
 * - Show filenames only (not full paths) for better readability
 */
class HistoryCommand : public Command {
public:
    /**
     * @brief Constructs a HistoryCommand with the provided configuration.
     * 
     * @param commandArgs Command arguments (unused for history command)
     * @param commandApiKey API key (unused for history command)
     * @param commandProvider Current provider name (unused for history command)
     * @param commandCurrentHistory Path to current history file
     * @param commandHistoryDir Path to history directory
     */
    HistoryCommand(const std::vector<std::string>& commandArgs,
                   const std::string& commandApiKey,
                   const std::string& commandProvider,
                   const std::string& commandCurrentHistory,
                   const std::string& commandHistoryDir);

    /**
     * @brief Executes the history listing command.
     * 
     * Lists all files in the history directory and displays the current
     * active history file. Files are shown as filenames only for better
     * readability.
     * 
     * @throws std::runtime_error if history directory doesn't exist or can't be read
     */
    void execute() override;

    /**
     * @brief Validates that the command can be executed.
     * 
     * Checks that the history directory path is provided and that the
     * directory exists and is accessible.
     * 
     * @throws std::invalid_argument if history directory is missing or inaccessible
     */
    void validateArgs() const override;

    /**
     * @brief Gets the command name.
     * 
     * @return "history"
     */
    std::string getCommandName() const override;

private:
    /**
     * @brief Parses and executes the appropriate subcommand.
     * 
     * This method examines the first argument to determine which subcommand
     * to execute. If no subcommand is provided, defaults to listing behavior.
     */
    void executeSubcommand();

    /**
     * @brief Executes the default list behavior (no subcommand).
     * 
     * Lists all files in the history directory and displays the current
     * active history file path.
     */
    void executeList();

    /**
     * @brief Executes the 'view' subcommand.
     * 
     * Displays the conversation content from a specified history file
     * in a user-friendly formatted display.
     * 
     * @param target The target to view ("current", "latest", or filename)
     */
    void executeView(const std::string& target);

    /**
     * @brief Executes the 'reuse' subcommand.
     * 
     * Switches the current active conversation to a specified history file,
     * allowing the user to continue a previous conversation.
     * 
     * @param target The target to reuse ("current", "latest", or filename)
     */
    void executeReuse(const std::string& target);

    /**
     * @brief Resolves a target string to an actual history file path.
     * 
     * @param target Target identifier ("current", "latest", or filename)
     * @return Full path to the resolved history file
     * @throws std::runtime_error if target cannot be resolved
     */
    std::string resolveTarget(const std::string& target) const;

    /**
     * @brief Extracts conversation name from a file path.
     * 
     * @param filePath Full path to the history file
     * @return Extracted conversation name
     */
    std::string extractConversationName(const std::string& filePath) const;

    /**
     * @brief Archives the current conversation if it exists and is different from the target.
     * 
     * @param newConversationName The name of the conversation being switched to
     */
    void archiveCurrentConversationIfNeeded(const std::string& newConversationName) const;

    /**
     * @brief Copies a history file to the current conversation format.
     * 
     * @param sourcePath Path to the source file
     * @param targetPath Path to the target file
     */
    void copyFileToCurrentFormat(const std::string& sourcePath, const std::string& targetPath) const;
};
