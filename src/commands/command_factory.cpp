#include "command_factory.h"
#include "list_command.h"
#include "history_command.h"
#include "chat_command.h"
#include "benchmark_command.h"
#include "blacklist_command.h"
#include "help_command.h"
#include "config_command.h"
#include <stdexcept>
#include <unordered_map>
#include <functional>

/**
 * Gets the static map of command creators.
 * This is the single source of truth for all supported commands.
 */
const std::unordered_map<std::string, CommandFactory::CommandCreator> &
CommandFactory::getCommandCreators()
{
    static const std::unordered_map<std::string, CommandCreator> creators = {
        {"list", [](const std::vector<std::string> &args, const ApplicationSetup::Config &config)
         {
             return std::make_unique<ListCommand>(args, config);
         }},
        {"history", [](const std::vector<std::string> &args, const ApplicationSetup::Config &config)
         {
             return std::make_unique<HistoryCommand>(args, config);
         }},
        {"benchmark", [](const std::vector<std::string> &args, const ApplicationSetup::Config &config)
         {
             return std::make_unique<BenchmarkCommand>(args, config);
         }},
        {"blacklist", [](const std::vector<std::string> &args, const ApplicationSetup::Config &config)
         {
             return std::make_unique<BlacklistCommand>(args, config);
         }},
        {"config", [](const std::vector<std::string> &args, const ApplicationSetup::Config &config)
         {
             return std::make_unique<ConfigCommand>(args, config);
         }},
        {"new", [](const std::vector<std::string> &args, const ApplicationSetup::Config &config)
         {
             return std::make_unique<ChatCommand>(args, config);
         }},
        {"help", [](const std::vector<std::string> &args, const ApplicationSetup::Config &config)
         {
             return std::make_unique<HelpCommand>(args, config);
         }}};
    return creators;
}

/**
 * Creates a command instance based on the command name.
 * Uses the factory function map for clean, O(1) command creation.
 */
std::unique_ptr<Command> CommandFactory::createCommand(
    const std::string &commandName,
    const std::vector<std::string> &commandArgs,
    const ApplicationSetup::Config &config)
{

    if (commandName.empty())
    {
        throw std::invalid_argument("Command name cannot be empty");
    }

    const auto &creators = getCommandCreators();
    auto it = creators.find(commandName);

    if (it != creators.end())
    {
        return it->second(commandArgs, config);
    }

    // Handle chat command for multi-word prompts
    if (isChatCommand(commandName))
    {
        // For multi-word prompts, create a ChatCommand with just the prompt
        // Don't include any additional arguments to avoid confusion
        std::vector<std::string> chatArgs = {commandName};
        return std::make_unique<ChatCommand>(chatArgs, config);
    }

    throw std::invalid_argument("Unknown command: " + commandName);
}

/**
 * Checks if a command name is valid and supported.
 */
bool CommandFactory::isValidCommand(const std::string &commandName)
{
    if (commandName.empty())
    {
        return false;
    }

    const auto &creators = getCommandCreators();
    return creators.find(commandName) != creators.end();
}

/**
 * Gets a list of all supported command names.
 */
std::vector<std::string> CommandFactory::getSupportedCommands()
{
    const auto &creators = getCommandCreators();
    std::vector<std::string> commands;
    commands.reserve(creators.size() + 1);

    // Extract keys from the map
    for (const auto &pair : creators)
    {
        commands.push_back(pair.first);
    }

    commands.push_back("chat"); // Add chat as a conceptual command
    return commands;
}

/**
 * Determines if the input represents a direct chat prompt.
 * Only treats multi-word input as chat commands to ensure
 * single-word unrecognized commands show errors.
 */
bool CommandFactory::isChatCommand(const std::string &input)
{
    const auto &creators = getCommandCreators();

    // If it's a known command, it's definitely not a chat command
    if (creators.find(input) != creators.end())
    {
        return false;
    }

    // Only treat as chat command if it contains spaces (multi-word)
    return input.find(' ') != std::string::npos;
}
