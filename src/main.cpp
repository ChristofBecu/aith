#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include "system_utils.h"
#include "provider_manager.h"
#include "model_blacklist.h"
#include "api.h"
#include "history.h"
#include "markdown.h"
#include "benchmark.h"
#include "commands/command_line_parser.h"
#include "application_setup.h"
#include "commands/command_factory.h"

int main(int argc, char *argv[]) {
    // Parse command line arguments
    auto parsedArgs = CommandLineParser::parseArguments(argc, argv);
    if (parsedArgs.hasError) {
        std::cerr << parsedArgs.errorMessage << std::endl;
        return 1;
    }
    
    // Get the remaining arguments after processing special flags
    std::vector<std::string> args = parsedArgs.remainingArgs;
    
    // Initialize application configuration
    ApplicationSetup::Config config;
    try {
        config = ApplicationSetup::initialize();
    } catch (const std::runtime_error& e) {
        // ApplicationSetup handles error messages, just return error code
        return 1;
    }

    if (args.empty()) {
        std::cerr << "Usage: aith [--provider=NAME | -p NAME] [list | history | test | blacklist | new \"prompt\" | \"prompt\"] [model (optional)]" << std::endl;
        return 1;
    }

    std::string command = args[0];
    
    // Use CommandFactory for implemented commands
    if (command == "list") {
        try {
            auto cmd = CommandFactory::createCommand(command, args, config);
            cmd->execute();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else if (command == "history") {
        try {
            auto cmd = CommandFactory::createCommand(command, args, config);
            cmd->execute();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else if (command == "test") {
        try {
            auto cmd = CommandFactory::createCommand(command, args, config);
            cmd->execute();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else if (command == "blacklist") {
        // Handle blacklist commands
        if (args.size() < 2) {
            std::cerr << "Usage: ./aith blacklist [add|remove|list]" << std::endl;
            return 1;
        }
        
        std::string blacklistCommand = args[1];
        
        if (blacklistCommand == "list") {
            // List all blacklisted models
            auto blacklistedModels = ModelBlacklist::getBlacklistedModels();
            
            if (blacklistedModels.empty()) {
                std::cout << "No models are currently blacklisted." << std::endl;
            } else {
                std::cout << "Blacklisted models:" << std::endl;
                for (const auto &entry : blacklistedModels) {
                    std::cout << "- " << entry.provider << " | " << entry.model;
                    if (!entry.reason.empty()) {
                        std::cout << " | Reason: " << entry.reason;
                    }
                    if (!entry.timestamp.empty()) {
                        std::cout << " | Added: " << entry.timestamp;
                    }
                    std::cout << std::endl;
                }
            }
        } else if (blacklistCommand == "add") {
            // Add a model to the blacklist
            if (args.size() < 4) {
                std::cerr << "Usage: aith blacklist add <provider> <model_name> [reason]" << std::endl;
                return 1;
            }
            
            std::string provider = args[2];
            std::string modelName = args[3];
            std::string reason = "";
            
            // Check if a reason was provided
            if (args.size() > 4) {
                reason = args[4];
                // Include all remaining arguments as part of the reason
                for (size_t i = 5; i < args.size(); ++i) {
                    reason += " " + args[i];
                }
            }
            
            ModelBlacklist::addModelToBlacklist(provider, modelName, reason);
        } else if (blacklistCommand == "remove") {
            // Remove a model from the blacklist
            if (args.size() < 4) {
                std::cerr << "Usage: ./aith blacklist remove <provider> <model_name>" << std::endl;
                return 1;
            }
            
            std::string provider = args[2];
            std::string modelName = args[3];
            ModelBlacklist::removeModelFromBlacklist(provider, modelName);
        } else {
            std::cerr << "Unknown blacklist command: " << blacklistCommand << std::endl;
            std::cerr << "Available commands: add, remove, list" << std::endl;
            return 1;
        }
    } else if (command == "new") {
        try {
            auto cmd = CommandFactory::createCommand(command, args, config);
            cmd->execute();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        // First argument is the prompt - treat as direct chat command
        try {
            auto cmd = CommandFactory::createCommand(command, args, config);
            cmd->execute();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}