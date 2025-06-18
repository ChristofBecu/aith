#include "conversation_displayer.h"
#include "json_file_handler.h"
#include "string_utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <filesystem>

/**
 * Displays a complete conversation from a JSON file
 */
void ConversationDisplayer::displayConversation(const std::string& filePath, bool showStats) {
    Json::Value conversation = JsonFileHandler::read(filePath);
    std::string filename = extractDisplayName(filePath);
    displayConversation(conversation, filename, showStats);
}

/**
 * Displays a conversation from a JSON array
 */
void ConversationDisplayer::displayConversation(const Json::Value& conversation, 
                                              const std::string& filename, 
                                              bool showStats) {
    if (!conversation.isArray()) {
        throw std::runtime_error("Invalid conversation format: expected JSON array");
    }
    
    int messageCount = static_cast<int>(conversation.size());
    
    // Display header
    displayHeader(filename, messageCount);
    
    // Display messages
    for (int i = 0; i < messageCount; ++i) {
        const Json::Value& message = conversation[i];
        displayMessage(message, i + 1);
        
        // Add spacing between messages (except after the last one)
        if (i < messageCount - 1) {
            std::cout << std::endl;
        }
    }
    
    // Display statistics if requested
    if (showStats) {
        std::cout << std::endl;
        displayStats(conversation);
    }
    
    // Display footer
    displayFooter();
}

/**
 * Displays a conversation header with file information
 */
void ConversationDisplayer::displayHeader(const std::string& filename, int messageCount) {
    std::cout << "\n";
    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n";
    std::cout << "🗨️  CONVERSATION: " << filename << "\n";
    std::cout << "📝 " << messageCount << " message" << (messageCount != 1 ? "s" : "") << "\n";
    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n";
    std::cout << std::endl;
}

/**
 * Displays a conversation footer
 */
void ConversationDisplayer::displayFooter() {
    std::cout << "\n═══════════════════════════════════════════════════════════════════════════════\n";
}

/**
 * Formats and displays a single message
 */
void ConversationDisplayer::displayMessage(const Json::Value& message, int messageIndex) {
    if (!message.isMember("role") || !message.isMember("content")) {
        std::cout << "⚠️  [Message " << messageIndex << "] Invalid message format\n";
        return;
    }
    
    std::string role = message["role"].asString();
    std::string content = message["content"].asString();
    
    // Display role indicator
    std::cout << getRoleIndicator(role) << " ";
    
    // Display formatted content
    std::string formattedContent = formatContent(content, 4, 76); // Leave room for role indicator
    std::cout << formattedContent << std::endl;
}

/**
 * Displays conversation statistics
 */
void ConversationDisplayer::displayStats(const Json::Value& conversation) {
    int messageCount = static_cast<int>(conversation.size());
    int userMessages = 0;
    int assistantMessages = 0;
    int systemMessages = 0;
    int characterCount = getCharacterCount(conversation);
    int wordCount = getWordCount(conversation);
    
    // Count message types
    for (const auto& message : conversation) {
        if (message.isMember("role")) {
            std::string role = message["role"].asString();
            if (role == "user") {
                userMessages++;
            } else if (role == "assistant") {
                assistantMessages++;
            } else if (role == "system") {
                systemMessages++;
            }
        }
    }
    
    std::cout << "┌─ CONVERSATION STATISTICS ─────────────────────────────────────────────────────┐\n";
    std::cout << "│ Total Messages: " << std::setw(8) << messageCount;
    std::cout << "  │  User: " << std::setw(4) << userMessages;
    std::cout << "  │  Assistant: " << std::setw(4) << assistantMessages;
    if (systemMessages > 0) {
        std::cout << "  │  System: " << std::setw(4) << systemMessages;
    }
    std::cout << "  │\n";
    std::cout << "│ Total Words: " << std::setw(11) << wordCount;
    std::cout << "  │  Characters: " << std::setw(8) << characterCount;
    std::cout << "                        │\n";
    std::cout << "└───────────────────────────────────────────────────────────────────────────────┘\n";
}

/**
 * Gets a role indicator with color formatting for terminal display
 */
std::string ConversationDisplayer::getRoleIndicator(const std::string& role) {
    if (role == "user") {
        return "\033[1;34m👤 USER:\033[0m";      // Bold blue
    } else if (role == "assistant") {
        return "\033[1;32m🤖 AI:\033[0m";        // Bold green
    } else if (role == "system") {
        return "\033[1;33m⚙️  SYSTEM:\033[0m";   // Bold yellow
    } else {
        return "\033[1;37m❓ " + role + ":\033[0m"; // Bold white for unknown roles
    }
}

/**
 * Formats content text for display with proper word wrapping
 */
std::string ConversationDisplayer::formatContent(const std::string& content, 
                                               int indent, 
                                               int maxWidth) {
    std::istringstream iss(content);
    std::string word;
    std::string line;
    std::string result;
    std::string indentStr(indent, ' ');
    
    bool firstLine = true;
    
    while (iss >> word) {
        // Check if adding this word would exceed the line width
        if (!line.empty() && line.length() + word.length() + 1 > static_cast<size_t>(maxWidth)) {
            // Add the current line to result
            if (!firstLine) {
                result += indentStr;
            }
            result += line + "\n";
            line = word;
            firstLine = false;
        } else {
            // Add word to current line
            if (!line.empty()) {
                line += " ";
            }
            line += word;
        }
    }
    
    // Add the final line
    if (!line.empty()) {
        if (!firstLine) {
            result += indentStr;
        }
        result += line;
    }
    
    return result;
}

/**
 * Extracts the display name from a filename
 */
std::string ConversationDisplayer::extractDisplayName(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string filename = path.filename().string();
    
    // Remove .json extension
    if (filename.length() > 5 && filename.substr(filename.length() - 5) == ".json") {
        filename = filename.substr(0, filename.length() - 5);
    }
    
    // Handle different filename patterns
    if (filename.substr(0, 8) == "current_") {
        // Remove "current_" prefix and return the rest
        return filename.substr(8);
    } else if (filename.substr(0, 8) == "history_") {
        // Remove "history_" prefix and timestamp suffix
        std::string withoutPrefix = filename.substr(8);
        
        // Find the last occurrence of timestamp pattern (_YYYYMMDD_HHMMSS)
        size_t timestampPos = withoutPrefix.rfind("_20");
        if (timestampPos != std::string::npos) {
            // Check if this looks like a timestamp
            std::string possibleTimestamp = withoutPrefix.substr(timestampPos);
            if (possibleTimestamp.length() >= 16) { // _YYYYMMDD_HHMMSS = 16 characters
                withoutPrefix = withoutPrefix.substr(0, timestampPos);
            }
        }
        
        return withoutPrefix;
    }
    
    // Return as-is if no pattern matches
    return filename;
}

/**
 * Gets the character count for a conversation
 */
int ConversationDisplayer::getCharacterCount(const Json::Value& conversation) {
    int count = 0;
    for (const auto& message : conversation) {
        if (message.isMember("content")) {
            count += static_cast<int>(message["content"].asString().length());
        }
    }
    return count;
}

/**
 * Gets the word count for a conversation
 */
int ConversationDisplayer::getWordCount(const Json::Value& conversation) {
    int count = 0;
    for (const auto& message : conversation) {
        if (message.isMember("content")) {
            std::string content = message["content"].asString();
            std::istringstream iss(content);
            std::string word;
            while (iss >> word) {
                count++;
            }
        }
    }
    return count;
}
