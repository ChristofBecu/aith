#ifndef CONVERSATION_DISPLAYER_H
#define CONVERSATION_DISPLAYER_H

#include <string>
#include <json/json.h>

/**
 * @brief Utility class for displaying conversation history in a formatted, user-friendly way
 * 
 * This class provides methods for:
 * - Formatting individual messages with role indicators
 * - Displaying conversation headers with metadata
 * - Showing conversation statistics
 * - Formatting complete conversations for terminal display
 * 
 * Follows Single Responsibility Principle by focusing only on conversation display operations.
 */
class ConversationDisplayer {
public:
    /**
     * Displays a complete conversation from a JSON file
     * @param filePath Path to the conversation JSON file
     * @param showStats Whether to display conversation statistics at the end
     * @throws std::runtime_error if file cannot be read or parsed
     */
    static void displayConversation(const std::string& filePath, bool showStats = true);
    
    /**
     * Displays a conversation from a JSON array
     * @param conversation JSON array containing the conversation messages
     * @param filename Optional filename to display in the header
     * @param showStats Whether to display conversation statistics at the end
     */
    static void displayConversation(const Json::Value& conversation, 
                                   const std::string& filename = "", 
                                   bool showStats = true);

private:
    /**
     * Displays a conversation header with file information
     * @param filename The conversation filename
     * @param messageCount Number of messages in the conversation
     */
    static void displayHeader(const std::string& filename, int messageCount);
    
    /**
     * Displays a conversation footer
     */
    static void displayFooter();
    
    /**
     * Formats and displays a single message
     * @param message JSON object containing role and content
     * @param messageIndex Index of the message in the conversation (1-based)
     */
    static void displayMessage(const Json::Value& message, int messageIndex);
    
    /**
     * Displays conversation statistics
     * @param conversation JSON array containing the conversation messages
     */
    static void displayStats(const Json::Value& conversation);
    
    /**
     * Gets a role indicator with color formatting for terminal display
     * @param role The message role (user, assistant, system)
     * @return Formatted role indicator string
     */
    static std::string getRoleIndicator(const std::string& role);
    
    /**
     * Formats content text for display with proper word wrapping
     * @param content The content text to format
     * @param indent Number of spaces to indent each line
     * @param maxWidth Maximum line width before wrapping
     * @return Formatted content string
     */
    static std::string formatContent(const std::string& content, 
                                   int indent = 4, 
                                   int maxWidth = 80);
    
    /**
     * Extracts the display name from a filename
     * Removes path, timestamp, and extension for cleaner display
     * @param filepath Full path to the conversation file
     * @return Clean display name for the conversation
     */
    static std::string extractDisplayName(const std::string& filepath);
    
    /**
     * Gets the character count for a conversation
     * @param conversation JSON array containing the conversation messages
     * @return Total character count across all messages
     */
    static int getCharacterCount(const Json::Value& conversation);
    
    /**
     * Gets the word count for a conversation
     * @param conversation JSON array containing the conversation messages
     * @return Total word count across all messages
     */
    static int getWordCount(const Json::Value& conversation);
};

#endif // CONVERSATION_DISPLAYER_H
