#pragma once

#include <vector>
#include <string>

/**
 * @brief Handles command line argument parsing for the AI application.
 * 
 * This class extracts and processes command line arguments, including
 * special arguments like --provider/-p, and returns the remaining
 * arguments for further command processing.
 */
class CommandLineParser {
public:
    /**
     * @brief Parsed command line arguments structure
     */
    struct ParsedArgs {
        std::vector<std::string> remainingArgs; ///< Arguments after processing special flags
        bool hasError = false;                  ///< Indicates if parsing encountered an error
        std::string errorMessage;               ///< Error message if hasError is true
    };

    /**
     * @brief Parses command line arguments and processes special flags.
     * 
     * Processes special arguments like --provider/--p and sets up the
     * ProviderManager accordingly. Returns the remaining arguments for
     * command processing.
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @return ParsedArgs structure containing processed results
     */
    static ParsedArgs parseArguments(int argc, char* argv[]);

private:
    /**
     * @brief Checks if an argument starts with a specific prefix.
     * @param arg The argument to check
     * @param prefix The prefix to look for
     * @return true if the argument starts with the prefix
     */
    static bool hasPrefix(const std::string& arg, const std::string& prefix);

    /**
     * @brief Extracts the value part after a prefix (e.g., --provider=groq -> groq).
     * @param arg The full argument string
     * @param prefix The prefix to remove
     * @return The value part after the prefix
     */
    static std::string extractValue(const std::string& arg, const std::string& prefix);
};
