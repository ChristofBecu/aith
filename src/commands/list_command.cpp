#include "commands/list_command.h"
#include "core/provider_manager.h"
#include "core/api_manager.h"
#include <iostream>
#include <stdexcept>

/**
 * @brief Constructs a ListCommand with the provided configuration.
 */
ListCommand::ListCommand(const std::vector<std::string>& commandArgs,
                        const ApplicationSetup::Config& config)
    : Command(commandArgs, config.apiKey, config.provider, config.currentHistory, config.historyDir) {
}

/**
 * @brief Executes the list models command.
 */
void ListCommand::execute() {
    // Validate arguments before execution
    validateArgs();
    
    // Display header with current provider name
    std::cout << "Available models for provider '" << ProviderManager::getAgent() << "':" << std::endl;
    
    // Use existing API infrastructure to list models
    ApiManager::listModels(apiKey);
}

/**
 * @brief Validates that the command can be executed.
 */
void ListCommand::validateArgs() const {
    if (apiKey.empty()) {
        throw std::invalid_argument("API key is required for listing models");
    }
}

/**
 * @brief Gets the command name.
 */
std::string ListCommand::getCommandName() const {
    return "list";
}
