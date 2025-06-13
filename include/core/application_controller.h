#ifndef APPLICATION_CONTROLLER_H
#define APPLICATION_CONTROLLER_H

#include <vector>
#include <string>
#include "application_setup.h"

/**
 * @class ApplicationController
 * @brief Main application controller that orchestrates the entire application flow
 * 
 * This class serves as the central coordinator for the AITH application.
 * It handles the complete application lifecycle including:
 * - Command line argument parsing
 * - Application initialization and setup
 * - Command execution via the CommandFactory
 * - Error handling and reporting
 * - Application exit codes
 * 
 * This design follows the Controller pattern, providing a clean separation
 * between the main entry point and the application logic.
 */
class ApplicationController {
public:
    /**
     * @brief Constructor for ApplicationController
     */
    ApplicationController() = default;
    
    /**
     * @brief Destructor for ApplicationController
     */
    ~ApplicationController() = default;
    
    /**
     * @brief Main application entry point
     * 
     * This method handles the complete application flow from command line
     * parsing to command execution. It provides centralized error handling
     * and ensures consistent behavior across all application operations.
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return Exit code (0 for success, non-zero for errors)
     */
    int run(int argc, char* argv[]);

private:
    /**
     * @brief Displays usage information to the user
     * 
     * Shows the correct command line syntax and available commands.
     */
    void displayUsage() const;
    
    /**
     * @brief Executes a command using the CommandFactory
     * 
     * This method provides a centralized way to execute commands with
     * consistent error handling and reporting.
     * 
     * @param command The command name to execute
     * @param args The complete argument list including the command
     * @param config The application configuration
     * @return true if command executed successfully, false otherwise
     */
    bool executeCommand(const std::string& command, 
                       const std::vector<std::string>& args,
                       const ApplicationSetup::Config& config) const;
    
    /**
     * @brief Reports an error to the user
     * 
     * Provides consistent error reporting format across the application.
     * 
     * @param message The error message to display
     */
    void reportError(const std::string& message) const;
    
    // Non-copyable and non-movable to ensure single instance semantics
    ApplicationController(const ApplicationController&) = delete;
    ApplicationController& operator=(const ApplicationController&) = delete;
    ApplicationController(ApplicationController&&) = delete;
    ApplicationController& operator=(ApplicationController&&) = delete;
};

#endif // APPLICATION_CONTROLLER_H
