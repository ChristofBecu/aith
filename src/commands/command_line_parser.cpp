#include "commands/command_line_parser.h"
#include "core/provider_manager.h"
#include <iostream>

/**
 * @brief Parses command line arguments and processes special flags.
 * 
 * Processes special arguments like --provider/--p and sets up the
 * ProviderManager accordingly. Returns the remaining arguments for
 * command processing.
 */
CommandLineParser::ParsedArgs CommandLineParser::parseArguments(int argc, char* argv[]) {
    ParsedArgs result;
    
    // Store all arguments in a vector for easier processing
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    // Process special arguments like --provider and help flags
    for (size_t i = 0; i < args.size(); ++i) {
        // Check for help flags
        if (args[i] == "--help" || args[i] == "-h") {
            result.showHelp = true;
            args.erase(args.begin() + i);
            --i; // Adjust index after removal
        }
        // Check for --provider=value or -p value format
        else if (hasPrefix(args[i], "--provider=")) {
            ProviderManager::setCommandLineProvider(extractValue(args[i], "--provider="));
            args.erase(args.begin() + i);
            --i; // Adjust index after removal
        } else if (args[i] == "--provider" || args[i] == "-p") {
            if (i + 1 < args.size()) {
                ProviderManager::setCommandLineProvider(args[i + 1]);
                args.erase(args.begin() + i, args.begin() + i + 2);
                --i; // Adjust index after removal
            } else {
                result.hasError = true;
                result.errorMessage = "Error: --provider or -p option requires a value";
                return result;
            }
        }
    }
    
    // Set the remaining arguments
    result.remainingArgs = args;
    return result;
}

/**
 * @brief Checks if an argument starts with a specific prefix.
 */
bool CommandLineParser::hasPrefix(const std::string& arg, const std::string& prefix) {
    return arg.substr(0, prefix.size()) == prefix;
}

/**
 * @brief Extracts the value part after a prefix (e.g., --provider=groq -> groq).
 */
std::string CommandLineParser::extractValue(const std::string& arg, const std::string& prefix) {
    return arg.substr(prefix.size());
}
