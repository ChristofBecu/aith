#pragma once

#include <string>
#include <vector>

/**
 * @brief Interactive configuration setup wizard
 * 
 * Provides a user-friendly way to set up AITH configuration from scratch.
 * This class contains the pure configuration logic, separate from command handling.
 * Guides users through provider selection, API key setup, and basic configuration.
 */
class ConfigWizard {
public:
    /**
     * @brief Run the interactive configuration wizard
     * 
     * Main entry point called by ConfigCommand::executeWizardCommand().
     * Guides the user through complete configuration setup:
     * 1. Provider selection
     * 2. API key input and validation
     * 3. Default model selection
     * 4. Configuration file generation
     * 
     * @param forceNonInteractive If true, skip the interactive terminal check
     */
    static void runInteractiveSetup(bool forceNonInteractive = false);

private:
    /**
     * @brief Display welcome message and instructions
     */
    static void displayWelcome();
    
    /**
     * @brief Prompt user to select an AI provider
     * @return Selected provider name
     */
    static std::string promptForProvider();
    
    /**
     * @brief Prompt user for API key for the given provider
     * @param provider The provider name
     * @return API key entered by user
     */
    static std::string promptForApiKey(const std::string& provider);
    
    /**
     * @brief Prompt user to select default model for provider
     * @param provider The provider name
     * @return Selected model name
     */
    static std::string promptForModel(const std::string& provider);
    
    /**
     * @brief Validate configuration and save to files
     * @param provider Provider name
     * @param apiKey API key
     * @param model Default model
     * @return True if configuration was saved successfully
     */
    static bool validateAndSaveConfiguration(const std::string& provider, 
                                           const std::string& apiKey, 
                                           const std::string& model);
    
    /**
     * @brief Display completion message with next steps
     * @param provider The configured provider
     */
    static void displayCompletion(const std::string& provider);
    
    /**
     * @brief Get user input with prompt
     * @param prompt Prompt message to display
     * @param defaultValue Default value if user presses enter
     * @return User input or default value
     * @throws std::runtime_error if input cannot be read (e.g., EOF in non-interactive mode)
     */
    static std::string getUserInput(const std::string& prompt, 
                                  const std::string& defaultValue = "");
    
    /**
     * @brief Display menu and get user selection
     * @param title Menu title
     * @param options List of menu options
     * @return Selected option index (0-based)
     * @throws std::runtime_error if input cannot be read
     */
    static int displayMenuAndGetSelection(const std::string& title, 
                                        const std::vector<std::string>& options);
    
    /**
     * @brief Check if we're running in an interactive terminal
     * @return True if stdin is connected to a terminal
     */
    static bool isInteractiveTerminal();
    
    /**
     * @brief Safely read a line from stdin with EOF handling
     * @param input Reference to store the input
     * @return True if input was successfully read, false on EOF
     */
    static bool safeGetline(std::string& input);
    
    /**
     * @brief Clear the terminal screen
     */
    static void clearScreen();
    
    /**
     * @brief Display a separator line
     */
    static void displaySeparator();
};
