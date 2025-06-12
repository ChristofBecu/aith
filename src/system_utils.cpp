#include "system_utils.h"
#include <cstdlib>
#include <stdexcept>
#include <array>
#include <memory>
#include <functional>
#include <iostream>

/**
 * Retrieves the value of an environment variable.
 * @param key The name of the environment variable.
 * @return The value of the environment variable, or an empty string if not found.
 */
std::string SystemUtils::getEnvVar(const std::string &key) {
    const char *val = std::getenv(key.c_str());
    if (val == nullptr) {
        return "";
    }
    return std::string(val);
}

/**
 * Executes a shell command and returns the output.
 * @param cmd The command to execute.
 * @return The output of the command as a string.
 */
std::string SystemUtils::exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, std::function<void(FILE*)>> pipe(
        popen(cmd, "r"), [](FILE* f) { pclose(f); }
    );
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

/**
 * Executes a shell command and checks its return code.
 * @param command The command to execute.
 * If the command fails, the program will terminate with an error message.
 */
void SystemUtils::executeCommand(const std::string &command) {
    int retCode = std::system(command.c_str());
    if (retCode != 0) {
        std::cerr << "Command failed: " << command << std::endl;
        std::exit(1);
    }
}
