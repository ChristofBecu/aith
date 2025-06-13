#pragma once

#include "command.h"

/**
 * @brief Command to list available AI models for the current provider.
 * 
 * This command displays all available models from the configured AI provider,
 * including blacklist status and other metadata. It leverages the existing
 * API infrastructure to fetch and display model information.
 * 
 * Usage:
 *   ./aith list
 * 
 * The command will:
 * - Validate API configuration
 * - Fetch models from the provider's API
 * - Display models with blacklist indicators
 * - Show model metadata (owner, creation date, etc.)
 */
class ListCommand : public Command {
public:
    /**
     * @brief Constructs a ListCommand with the provided configuration.
     * 
     * @param commandArgs Command arguments (unused for list command)
     * @param commandApiKey API key for provider authentication
     * @param commandProvider Current provider name
     * @param commandCurrentHistory Path to current history file (unused)
     * @param commandHistoryDir Path to history directory (unused)
     */
    ListCommand(const std::vector<std::string>& commandArgs,
                const std::string& commandApiKey,
                const std::string& commandProvider,
                const std::string& commandCurrentHistory,
                const std::string& commandHistoryDir);

    /**
     * @brief Executes the list models command.
     * 
     * Displays a header with the current provider name and then
     * lists all available models using the existing API infrastructure.
     * Models are displayed with blacklist status and metadata.
     * 
     * @throws std::runtime_error if API key is missing or API call fails
     */
    void execute() override;

    /**
     * @brief Validates that the command can be executed.
     * 
     * Checks that the API key is provided, as it's required for
     * authenticating with the provider's API.
     * 
     * @throws std::invalid_argument if API key is missing or empty
     */
    void validateArgs() const override;

    /**
     * @brief Gets the command name.
     * 
     * @return "list"
     */
    std::string getCommandName() const override;
};
