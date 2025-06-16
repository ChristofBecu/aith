#ifndef MARKDOWN_RENDERER_H
#define MARKDOWN_RENDERER_H

#include <string>
#include "md4c.h"

/**
 * @brief C++ markdown renderer for terminal output with ANSI colors.
 * 
 * This class replaces the external mdcat dependency by providing
 * native C++ markdown rendering using the md4c library with custom
 * ANSI terminal formatting for better readability.
 */
class TerminalMarkdownRenderer {
public:
    /**
     * @brief Renders markdown text to ANSI-formatted terminal output.
     * @param markdown The markdown text to render
     * @return Formatted string with ANSI color codes for terminal display
     */
    std::string render(const std::string& markdown);

private:
    // ANSI color codes for terminal formatting
    static const std::string RESET;
    static const std::string BOLD;
    static const std::string DIM;
    static const std::string UNDERLINE;
    
    // Color definitions
    static const std::string BLACK;
    static const std::string RED;
    static const std::string GREEN;
    static const std::string YELLOW;
    static const std::string BLUE;
    static const std::string MAGENTA;
    static const std::string CYAN;
    static const std::string WHITE;
    
    // Background colors
    static const std::string BG_BLACK;
    static const std::string BG_GRAY;
    
    /**
     * @brief Internal renderer state for tracking output formatting.
     */
    struct RenderState {
        std::string output;
        int indentLevel;
        bool inCodeBlock;
        bool inList;
        int listLevel;
        
        RenderState() : indentLevel(0), inCodeBlock(false), inList(false), listLevel(0) {}
    };
    
    // Static callback functions for md4c parser
    static int enterBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata);
    static int leaveBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata);
    static int enterSpanCallback(MD_SPANTYPE spanType, void* detail, void* userdata);
    static int leaveSpanCallback(MD_SPANTYPE spanType, void* detail, void* userdata);
    static int textCallback(MD_TEXTTYPE textType, const MD_CHAR* text, MD_SIZE size, void* userdata);
    
    // Static helper methods for formatting (since callbacks are static)
    static void addIndentation(RenderState& state);
    static std::string getHeaderColor(int level);
    static std::string escapeAnsiSequences(const std::string& text);
};

#endif // MARKDOWN_RENDERER_H
