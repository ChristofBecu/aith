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
        try {
            auto cmd = CommandFactory::createCommand(command, args, config);
            cmd->execute();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
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