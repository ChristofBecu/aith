#ifndef TERMINAL_MARKDOWN_RENDERER_H
#define TERMINAL_MARKDOWN_RENDERER_H

#include <string>
#include <vector>
#include <memory>
#include "md4c.h"
#include "markdown/common/render_state.h"
#include "markdown/common/ansi_colors.h"
#include "markdown/common/text_utils.h"

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
    // Type aliases for cleaner code
    using RenderState = markdown::RenderState;
    using TableState = markdown::TableState;
    
    // Static callback functions for md4c parser
    static int enterBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata);
    static int leaveBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata);
    static int enterSpanCallback(MD_SPANTYPE spanType, void* detail, void* userdata);
    static int leaveSpanCallback(MD_SPANTYPE spanType, void* detail, void* userdata);
    static int textCallback(MD_TEXTTYPE textType, const MD_CHAR* text, MD_SIZE size, void* userdata);
    
    // Static helper methods for formatting (since callbacks are static)
    static void addIndentation(RenderState& state);
    static void addBlockquotePrefixes(RenderState& state);
    
    // Table rendering helpers
    static void calculateColumnWidths(TableState& table);
    static void renderTable(RenderState& state);
};

#endif // TERMINAL_MARKDOWN_RENDERER_H
