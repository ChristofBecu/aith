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
    
    // TODO: In future phases, replace this with CommandFactory::createCommand()
    // For now, CommandFactory is ready but we preserve original behavior
    // where unrecognized commands are treated as chat prompts
    
    if (command == "list") {
        std::cout << "Available models for provider '" << ProviderManager::getAgent() << "':" << std::endl;
        listModels(config.apiKey);
    } else if (command == "history") {
        for (const auto &entry : std::filesystem::directory_iterator(config.historyDir)) {
            std::cout << entry.path().filename().string() << std::endl;
        }
        std::cout << "\n Current history file: " << config.currentHistory << std::endl;
    } else if (command == "test") {
        // Handle benchmark testing
        std::string testPrompt = "Hello";
        
        // Check if custom test prompt was provided
        if (args.size() > 1) {
            testPrompt = args[1];
        }
        
        std::cout << "Starting model benchmark tests..." << std::endl;
        auto results = runAllModelsBenchmark(config.apiKey, testPrompt);
        displayBenchmarkResults(results);
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
        if (args.size() < 2) {
            std::cerr << "Usage: ./aith [--provider=NAME] new \"prompt\"" << std::endl;
            return 1;
        }
        std::string prompt = args[1];
        startNewHistory(prompt, config.historyDir, config.currentHistory);
        chat(prompt, args.size() > 2 ? args[2] : "", config.apiKey, config.currentHistory, true);
    } else {
        // First argument is the prompt
        std::string prompt = args[0];
        chat(prompt, args.size() > 1 ? args[1] : "", config.apiKey, config.currentHistory, false);
    }

    return 0;
}