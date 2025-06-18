#ifndef TERMINAL_MARKDOWN_RENDERER_H
#define TERMINAL_MARKDOWN_RENDERER_H

#include <string>
#include <vector>
#include <memory>
#include "md4c.h"
#include "markdown/common/render_state.h"
#include "markdown/common/ansi_colors.h"
#include "markdown/common/text_utils.h"
#include "markdown/common/word_wrapper.h"
#include "markdown/factory/block_handler_factory.h"

/**
 * @brief C++ markdown renderer for terminal output with ANSI colors.
 * 
 * This class replaces the external mdcat dependency by providing
 * native C++ markdown rendering using the md4c library with custom
 * ANSI terminal formatting for better readability.
 * 
 * The renderer uses a modular design with separate block handlers
 * for different markdown elements, managed through a factory pattern.
 */
class TerminalMarkdownRenderer {
public:
    /**
     * @brief Constructor initializes the block handler factory
     */
    TerminalMarkdownRenderer();
    
    /**
     * @brief Renders markdown text to ANSI-formatted terminal output.
     * @param markdown The markdown text to render
     * @return Formatted string with ANSI color codes for terminal display
     */
    std::string render(const std::string& markdown);
    
    /**
     * @brief Configure word wrapping settings
     * @param enabled Whether to enable word wrapping
     */
    void setWordWrappingEnabled(bool enabled) { wordWrapper_.setEnabled(enabled); }
    
    /**
     * @brief Set custom terminal width for word wrapping
     * @param width Terminal width in characters (0 for auto-detection)
     */
    void setCustomTerminalWidth(size_t width) { wordWrapper_.setTerminalWidth(width); }
    
    /**
     * @brief Check if word wrapping is enabled
     * @return True if word wrapping is enabled
     */
    bool isWordWrappingEnabled() const { return wordWrapper_.isEnabled(); }
    
    /**
     * @brief Get the custom terminal width setting
     * @return Custom terminal width (0 means auto-detection)
     */
    size_t getCustomTerminalWidth() const { return wordWrapper_.getTerminalWidth(); }

private:
    // Type aliases for cleaner code
    using RenderState = markdown::RenderState;
    using TableState = markdown::TableState;
    
    // Block handler factory for delegating block processing
    markdown::BlockHandlerFactory blockHandlerFactory;
    
    // Word wrapper for post-processing text wrapping
    markdown::WordWrapper wordWrapper_;
    
    // Static callback functions for md4c parser
    static int enterBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata);
    static int leaveBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata);
    static int enterSpanCallback(MD_SPANTYPE spanType, void* detail, void* userdata);
    static int leaveSpanCallback(MD_SPANTYPE spanType, void* detail, void* userdata);
    static int textCallback(MD_TEXTTYPE textType, const MD_CHAR* text, MD_SIZE size, void* userdata);
    
    // Static helper methods for formatting (since callbacks are static)
    static void addIndentation(RenderState& state);
    static void addBlockquotePrefixes(RenderState& state);
};

#endif // TERMINAL_MARKDOWN_RENDERER_H
