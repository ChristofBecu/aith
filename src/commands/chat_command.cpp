#include "chat_command.h"
#include "api_manager.h"
#include "chat/history.h"
#include <iostream>
#include <stdexcept>

ChatCommand::ChatCommand(const std::vector<std::string>& args, const ApplicationSetup::Config& config)
    : Command(args, config.apiKey, config.provider, config.currentHistory, config.historyDir),
      args_(args), config_(config) {
}

void ChatCommand::execute() {
    validateArgs();
    
    if (args_.empty()) {
        throw std::runtime_error("Chat command requires arguments");
    }
    
    std::string command = args_[0];
    
    if (command == "new") {
        // Handle new conversation command
        if (args_.size() < 2) {
            throw std::runtime_error("Usage: aith [--provider=NAME] new \"prompt\" [model]");
        }
        
        std::string prompt = args_[1];
        std::string model = args_.size() > 2 ? args_[2] : "";
        executeNewChat(prompt, model);
    } else {
        // Handle direct chat command - first argument is the prompt
        std::string prompt = args_[0];
        std::string model = args_.size() > 1 ? args_[1] : "";
        executeDirectChat(prompt, model);
    }
}

void ChatCommand::validateArgs() const {
    if (args_.empty()) {
        throw std::invalid_argument("Chat command requires at least one argument (prompt or 'new')");
    }
    
    std::string command = args_[0];
    
    if (command == "new") {
        if (args_.size() < 2) {
            throw std::invalid_argument("New conversation command requires a prompt argument");
        }
        if (args_[1].empty()) {
            throw std::invalid_argument("Prompt cannot be empty");
        }
    } else {
        // Direct chat - first argument should be the prompt
        if (args_[0].empty()) {
            throw std::invalid_argument("Chat prompt cannot be empty");
        }
    }
}

std::string ChatCommand::getCommandName() const {
    if (!args_.empty() && args_[0] == "new") {
        return "new";
    }
    return "chat";
}

void ChatCommand::executeDirectChat(const std::string& prompt, const std::string& model) {
    ApiManager::chat(prompt, model, config_.apiKey, config_.currentHistory, false);
}

void ChatCommand::executeNewChat(const std::string& prompt, const std::string& model) {
    // Start new history and get the updated current history path
    std::string newCurrentHistoryPath = startNewHistoryAndGetPath(prompt, config_.historyDir, config_.currentHistory);
    
    // Update our config to use the new path
    config_.currentHistory = newCurrentHistoryPath;
    
    ApiManager::chat(prompt, model, config_.apiKey, config_.currentHistory, true);
}
