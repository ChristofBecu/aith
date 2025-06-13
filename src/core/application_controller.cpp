#include "core/application_controller.h"
#include "commands/command_line_parser.h"
#include "core/application_setup.h"
#include "commands/command_factory.h"
#include <iostream>

int ApplicationController::run(int argc, char* argv[]) {
    // Parse command line arguments
    auto parsedArgs = CommandLineParser::parseArguments(argc, argv);
    if (parsedArgs.hasError) {
        reportError(parsedArgs.errorMessage);
        return 1;
    }
    
    // Get the remaining arguments after processing special flags
    std::vector<std::string> args = parsedArgs.remainingArgs;
    
    // Validate that we have at least one argument (the command)
    if (args.empty()) {
        displayUsage();
        return 1;
    }
    
    // Initialize application configuration
    ApplicationSetup::Config config;
    try {
        config = ApplicationSetup::initialize();
    } catch (const std::runtime_error& e) {
        // ApplicationSetup handles error messages, just return error code
        return 1;
    }
    
    // Extract and execute the command
    std::string command = args[0];
    if (!executeCommand(command, args, config)) {
        return 1;
    }
    
    return 0;
}

void ApplicationController::displayUsage() const {
    std::cerr << "Usage: aith [--provider=NAME | -p NAME] [list | history | test | blacklist | new \"prompt\" | \"prompt\"] [model (optional)]" << std::endl;
}

bool ApplicationController::executeCommand(const std::string& command, 
                                         const std::vector<std::string>& args,
                                         const ApplicationSetup::Config& config) const {
    try {
        auto cmd = CommandFactory::createCommand(command, args, config);
        cmd->execute();
        return true;
    } catch (const std::exception& e) {
        reportError(e.what());
        return false;
    }
}

void ApplicationController::reportError(const std::string& message) const {
    std::cerr << "Error: " << message << std::endl;
}
