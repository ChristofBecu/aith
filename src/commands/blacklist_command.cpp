#include "commands/blacklist_command.h"
#include "core/model_blacklist.h"
#include <iostream>
#include <stdexcept>

BlacklistCommand::BlacklistCommand(const std::vector<std::string>& args, const ApplicationSetup::Config& config)
    : Command(args, config.apiKey, config.provider, config.currentHistory, config.historyDir),
      args_(args), config_(config) {
}

void BlacklistCommand::execute() {
    validateArgs();
    
    std::string subCommand = getSubCommand();
    
    if (subCommand == "list") {
        executeListCommand();
    } else if (subCommand == "add") {
        executeAddCommand();
    } else if (subCommand == "remove") {
        executeRemoveCommand();
    } else {
        throw std::runtime_error("Unknown blacklist command: " + subCommand + 
                                ". Available commands: add, remove, list");
    }
}

void BlacklistCommand::validateArgs() const {
    if (args_.size() < 2) {
        throw std::invalid_argument("Usage: aith blacklist [add|remove|list]");
    }
    
    std::string subCommand = getSubCommand();
    
    if (subCommand == "add") {
        validateAddCommandArgs();
    } else if (subCommand == "remove") {
        validateRemoveCommandArgs();
    } else if (subCommand == "list") {
        // List command requires no additional arguments
    } else {
        throw std::invalid_argument("Unknown blacklist command: " + subCommand + 
                                  ". Available commands: add, remove, list");
    }
}

std::string BlacklistCommand::getCommandName() const {
    return "blacklist";
}

void BlacklistCommand::executeListCommand() {
    auto blacklistedModels = ModelBlacklist::getBlacklistedModels();
    
    if (blacklistedModels.empty()) {
        std::cout << "No models are currently blacklisted." << std::endl;
    } else {
        std::cout << "Blacklisted models:" << std::endl;
        for (const auto& entry : blacklistedModels) {
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
}

void BlacklistCommand::executeAddCommand() {
    std::string provider = args_[2];
    std::string modelName = args_[3];
    std::string reason = "";
    
    // Check if a reason was provided
    if (args_.size() > 4) {
        reason = buildReasonString(4);
    }
    
    ModelBlacklist::addModelToBlacklist(provider, modelName, reason);
}

void BlacklistCommand::executeRemoveCommand() {
    std::string provider = args_[2];
    std::string modelName = args_[3];
    
    ModelBlacklist::removeModelFromBlacklist(provider, modelName);
}

std::string BlacklistCommand::getSubCommand() const {
    if (args_.size() < 2) {
        return "";
    }
    return args_[1];
}

void BlacklistCommand::validateAddCommandArgs() const {
    if (args_.size() < 4) {
        throw std::invalid_argument("Usage: aith blacklist add <provider> <model_name> [reason]");
    }
    
    if (args_[2].empty()) {
        throw std::invalid_argument("Provider name cannot be empty");
    }
    
    if (args_[3].empty()) {
        throw std::invalid_argument("Model name cannot be empty");
    }
}

void BlacklistCommand::validateRemoveCommandArgs() const {
    if (args_.size() < 4) {
        throw std::invalid_argument("Usage: aith blacklist remove <provider> <model_name>");
    }
    
    if (args_[2].empty()) {
        throw std::invalid_argument("Provider name cannot be empty");
    }
    
    if (args_[3].empty()) {
        throw std::invalid_argument("Model name cannot be empty");
    }
}

std::string BlacklistCommand::buildReasonString(size_t startIndex) const {
    if (startIndex >= args_.size()) {
        return "";
    }
    
    std::string reason = args_[startIndex];
    
    // Include all remaining arguments as part of the reason
    for (size_t i = startIndex + 1; i < args_.size(); ++i) {
        reason += " " + args_[i];
    }
    
    return reason;
}
