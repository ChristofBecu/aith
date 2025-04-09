#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string getEnvVar(const std::string &key);
std::string exec(const char* cmd);
void executeCommand(const std::string &command);

#endif // UTILS_H