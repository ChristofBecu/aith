#pragma once

#include <string>

/**
 * @brief System utilities for environment variables and command execution.
 * 
 * This class provides static methods for common system operations like
 * reading environment variables and executing shell commands.
 */
class SystemUtils {
public:
    /**
     * Retrieves the value of an environment variable.
     * @param key The name of the environment variable.
     * @return The value of the environment variable, or an empty string if not found.
     */
    static std::string getEnvVar(const std::string &key);
    
    /**
     * Executes a shell command and returns the output.
     * @param cmd The command to execute.
     * @return The output of the command as a string.
     * @throws std::runtime_error if popen() fails
     */
    static std::string exec(const char* cmd);
    
    /**
     * Executes a shell command and checks its return code.
     * @param command The command to execute.
     * @throws std::runtime_error if command fails
     */
    static void executeCommand(const std::string &command);
};
