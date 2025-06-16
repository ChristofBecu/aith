#pragma once

#include "command.h"

/**
 * @brief Command to display conversation history files and current history.
 * 
 * This command lists all history files in the configured history directory
 * and shows the path to the current active history file. It provides users
 * with an overview of their conversation history files.
 * 
 * Usage:
 *   ./aith history
 * 
 * The command will:
 * - List all files in the history directory
 * - Display the current active history file path
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
};
