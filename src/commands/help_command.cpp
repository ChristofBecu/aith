#include "help_command.h"
#include <iostream>
#include <iomanip>

HelpCommand::HelpCommand(const std::vector<std::string>& args, const ApplicationSetup::Config& config)
    : Command(args, config.apiKey, config.provider, config.currentHistory, config.historyDir),
      config_(config) {
}

void HelpCommand::execute() {
    displayMainHelp();
    displayExamples();
    displayProviderInfo();
    // displayHistoryInfo();
}

void HelpCommand::validateArgs() const {
    // Help command doesn't require argument validation
    // It can be called with or without arguments
}

std::string HelpCommand::getCommandName() const {
    return "help";
}

void HelpCommand::displayMainHelp() const {
    std::cout << R"(
🤖 AITH - And It Talks, Human

USAGE:
    aith [OPTIONS] COMMAND [ARGUMENTS]

COMMANDS:
    new "prompt"              Start a new conversation with the given prompt
    "prompt"                  Continue the current conversation with a prompt
    list                      List available AI models for the current provider
    history                   Manage conversation history
    benchmark, test           Run performance benchmarks against AI providers
    blacklist                 Manage blacklisted models
    config                    Manage configuration settings
    help                      Display this help message

OPTIONS:
    --provider=NAME, -p NAME  Use a specific AI provider (groq, openrouter, etc.)

GLOBAL FLAGS:
    --help, -h               Show help information
    --provider=NAME          Override the default AI provider
    -p NAME                  Short form of --provider

)" << std::endl;
}

void HelpCommand::displayExamples() const {
    std::cout << R"(📚 EXAMPLES:

Basic Conversations:
    aith new "Explain quantum computing"
    aith "What are the practical applications?"
    aith new "How to debug C++ memory leaks?"

Using Different Providers:
    aith --provider=groq new "Write a Python function"
    aith -p openrouter "Optimize this SQL query"

Core Commands:
    aith list                           # List available models
    aith benchmark                      # Test provider performance
    aith help                           # Show help information

History management:
    aith history list                   # List all history files
    aith history view current           # View the current conversation
    aith history view latest            # View the previous conversation
    aith history view filename.json     # View conversation content
    aith history reuse latest           # Switch to previous conversation
    aith history reuse filename.json    # Switch to previous conversation

Blacklist management:
    aith blacklist add model-name       # Add a model to the blacklist
    aith blacklist remove model-name    # Remove a model from the blacklist
    aith blacklist list                 # List all blacklisted models

Configuration Management:
    aith config wizard                  # Run interactive setup wizard
    aith config list                    # List all configurations
    aith config show                    # Show current configuration
    aith config show anthropic          # Show specific provider config
    aith config add groq sk-xxx         # Add provider configuration
    aith config create groq             # Create a new provider configuration
    aith config edit groq               # Edit existing provider configuration
    aith config remove groq             # Remove provider configuration
    aith config validate                # Validate all configurations
    aith config validate groq           # Validate specific provider config
    aith config set-default [name]      # Change default provider
    aith config backup [name]           # Create configuration backup
    aith config restore [name]          # Restore from backup

)" << std::endl;
}

void HelpCommand::displayProviderInfo() const {
    std::cout << R"(🔌 AI PROVIDERS:

AITH supports multiple AI providers. Configure them via environment variables
or configuration files in ~/.config/aith/

Common Providers:
    groq            Fast inference with Groq's API
    openrouter      Access to multiple models via OpenRouter
    openai          Direct OpenAI API access
    anthropic       Claude models via Anthropic API

Configuration:
    Environment:    export GROQ_API_KEY="your-key-here"
    Config file:    ~/.config/aith/groq.conf
    
    Set default provider in ~/.config/aith/config:
    provider=groq

Current Provider: )" << config_.provider << R"(
Current API Key:  )" << (config_.apiKey.empty() ? "Not configured" : "Configured") << R"(

)" << std::endl;
}

void HelpCommand::displayHistoryInfo() const {
    std::cout << R"(📁 CONVERSATION MANAGEMENT:

AITH automatically manages your conversation history with descriptive filenames:

File Structure:
    ~/.config/aith/
    ├── current_conversation           # Tracks active conversation name
    ├── current_debug_memory_leaks.json    # Current conversation file
    └── aith_histories/
        ├── history_quantum_computing_20241218_143022.json
        ├── history_python_optimization_20241218_144530.json
        └── ...

How it Works:
    1. Start new conversation: aith new "Debug memory leaks"
       → Creates: current_debug_memory_leaks.json
    
    2. Continue conversation: aith "What tools to use?"
       → Appends to: current_debug_memory_leaks.json
    
    3. Start another conversation: aith new "Python optimization tips"
       → Archives: current_debug_memory_leaks.json → history_debug_memory_leaks_YYYYMMDD_HHMMSS.json
       → Creates: current_python_optimization_tips.json

Benefits:
    ✓ Know your current conversation topic at a glance
    ✓ Meaningful filenames for easy conversation discovery
    ✓ Automatic archiving with timestamps
    ✓ Seamless conversation switching

)" << std::endl;
}
