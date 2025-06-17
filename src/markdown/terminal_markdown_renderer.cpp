#include "markdown/terminal_markdown_renderer.h"
#include <md4c.h>
#include <iostream>
#include <sstream>
#include <algorithm>

/**
 * @brief Enhanced markdown renderer with comprehensive block support and professional table formatting
 * 
 * Supported Blocks:
 * - Headers (H1-H6): Hierarchical white/yellow/green/cyan colors with bold formatting
 * - Code blocks: Green language indicators, no background for better readability
 * - Paragraphs: Clean default text
 * - Unordered lists: Dim white bullet points (•)
 * - Ordered lists: Dim white numbered items (1., 2., etc.) with proper nesting
 * - Blockquotes: Vertical bar prefix (│) with proper multi-line support and nesting
 * - Horizontal rules: Dim white separator lines (────)
 * - Inline code: Dim cyan highlighting without background
 * - Tables: Professional box-drawing format with auto-sizing columns and header separation
 * 
 * Table Features:
 * - Box drawing characters (┌─┬─┐ │ ├─┼─┤ └─┴─┘) for professional appearance
 * - Automatic column width calculation based on content
 * - Proper cell padding and alignment
 * - Bold headers with separator line
 * - UTF-8 character width awareness
 * - Works within blockquotes and other contexts
 * 
 * Color Philosophy:
 * - Headers: Use hierarchical white/yellow/green/cyan for clear visual hierarchy
 * - Code elements: Readable without jarring backgrounds
 * - Lists: Subtle dim white markers
 * - Structural elements: Consistent dim white styling
 * - Tables: Elegant box drawing with bold headers
 * - Blockquotes: Proper prefix handling for multi-line content
 * - Terminal-friendly colors that work well in both light/dark themes
 */

// Use common components
using namespace markdown;

/**
 * Constructor initializes the block handler factory
 */
TerminalMarkdownRenderer::TerminalMarkdownRenderer() {
    // Factory is initialized automatically
}

/**
 * Renders markdown text to ANSI-formatted terminal output.
 */
std::string TerminalMarkdownRenderer::render(const std::string& markdown) {
    // First decode JSON escapes and Unicode HTML entities
    std::string decodedMarkdown = TextUtils::decodeJsonAndUnicodeEscapes(markdown);
    
    RenderState state;
    state.blockHandlerFactory = &blockHandlerFactory;
    
    // Configure md4c parser
    MD_PARSER parser = {
        0, // abi_version
        MD_FLAG_STRIKETHROUGH | MD_FLAG_TABLES | MD_FLAG_TASKLISTS, // flags
        enterBlockCallback,
        leaveBlockCallback,
        enterSpanCallback,
        leaveSpanCallback,
        textCallback,
        nullptr, // debug_log
        nullptr  // syntax
    };
    
    // Parse the decoded markdown
    int result = md_parse(decodedMarkdown.c_str(), decodedMarkdown.size(), &parser, &state);
    
    if (result != 0) {
        // Fallback to plain text if parsing fails
        return decodedMarkdown + "\n";
    }

    return state.output;
}

/**
 * Callback for entering block elements (headers, paragraphs, code blocks, etc.)
 */
int TerminalMarkdownRenderer::enterBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata) {
    auto* state = static_cast<RenderState*>(userdata);
    
    // Delegate to the factory if available
    if (state->blockHandlerFactory) {
        return state->blockHandlerFactory->handleEnterBlock(blockType, detail, *state);
    }
    
    // Fallback for any blocks not handled by the factory (shouldn't happen)
    return 0;
}

/**
 * Callback for leaving block elements
 */
int TerminalMarkdownRenderer::leaveBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata) {
    auto* state = static_cast<RenderState*>(userdata);
    
    // Delegate to the factory if available
    if (state->blockHandlerFactory) {
        return state->blockHandlerFactory->handleLeaveBlock(blockType, detail, *state);
    }
    
    // Fallback for any blocks not handled by the factory (shouldn't happen)
    return 0;
}

/**
 * Callback for entering span elements (emphasis, links, code, etc.)
 */
int TerminalMarkdownRenderer::enterSpanCallback(MD_SPANTYPE spanType, void* detail, void* userdata) {
    auto* state = static_cast<RenderState*>(userdata);
    
    switch (spanType) {
        case MD_SPAN_EM:
            state->output += AnsiColors::DIM;
            break;
        case MD_SPAN_STRONG:
            state->output += AnsiColors::BOLD;
            break;
        case MD_SPAN_CODE:
            state->output += AnsiColors::DIM + AnsiColors::CYAN;
            break;
        case MD_SPAN_A: {
            auto* linkDetail = static_cast<MD_SPAN_A_DETAIL*>(detail);
            state->output += AnsiColors::BLUE + AnsiColors::UNDERLINE;
            break;
        }
        case MD_SPAN_DEL:
            state->output += AnsiColors::DIM;
            break;
    }
    
    return 0;
}

/**
 * Callback for leaving span elements
 */
int TerminalMarkdownRenderer::leaveSpanCallback(MD_SPANTYPE spanType, void* detail, void* userdata) {
    auto* state = static_cast<RenderState*>(userdata);
    
    switch (spanType) {
        case MD_SPAN_EM:
        case MD_SPAN_STRONG:
        case MD_SPAN_CODE:
        case MD_SPAN_A:
        case MD_SPAN_DEL:
            state->output += AnsiColors::RESET;
            break;
    }
    
    return 0;
}

/**
 * Callback for text content
 */
int TerminalMarkdownRenderer::textCallback(MD_TEXTTYPE textType, const MD_CHAR* text, MD_SIZE size, void* userdata) {
    auto* state = static_cast<RenderState*>(userdata);
    
    std::string textStr(text, size);
    
    switch (textType) {
        case MD_TEXT_NORMAL:
        case MD_TEXT_ENTITY:
        case MD_TEXT_CODE:
            if (state->currentTable) {
                // Capture text content for table cells
                state->currentTable->currentCellContent += textStr;
            } else {
                state->output += AnsiColors::escapeAnsiSequences(textStr);
            }
            break;
        case MD_TEXT_NULLCHAR:
            // Skip null characters
            break;
        case MD_TEXT_BR:
        case MD_TEXT_SOFTBR:
            if (state->currentTable) {
                // Handle line breaks within table cells
                state->currentTable->currentCellContent += " ";
            } else {
                state->output += "\n";
                if (state->inCodeBlock) {
                    addIndentation(*state);
                } else if (state->blockquoteLevel > 0) {
                    addBlockquotePrefixes(*state);
                    addIndentation(*state);
                }
            }
            break;
    }
    
    return 0;
}

/**
 * Adds appropriate indentation based on current state
 */
void TerminalMarkdownRenderer::addIndentation(RenderState& state) {
    // Add regular indentation
    for (int i = 0; i < state.indentLevel; i++) {
        state.output += "  ";
    }
}

/**
 * Adds blockquote prefixes based on current blockquote nesting level
 */
void TerminalMarkdownRenderer::addBlockquotePrefixes(RenderState& state) {
    for (int i = 0; i < state.blockquoteLevel; i++) {
        state.output += AnsiColors::DIM + AnsiColors::WHITE + "│ " + AnsiColors::RESET;
    }
}
