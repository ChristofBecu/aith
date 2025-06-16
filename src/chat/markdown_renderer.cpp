#include "markdown_renderer.h"
#include <md4c.h>
#include <iostream>
#include <sstream>
#include <algorithm>

// ANSI color code definitions
const std::string TerminalMarkdownRenderer::RESET = "\033[0m";
const std::string TerminalMarkdownRenderer::BOLD = "\033[1m";
const std::string TerminalMarkdownRenderer::DIM = "\033[2m";
const std::string TerminalMarkdownRenderer::UNDERLINE = "\033[4m";

const std::string TerminalMarkdownRenderer::BLACK = "\033[30m";
const std::string TerminalMarkdownRenderer::RED = "\033[31m";
const std::string TerminalMarkdownRenderer::GREEN = "\033[32m";
const std::string TerminalMarkdownRenderer::YELLOW = "\033[33m";
const std::string TerminalMarkdownRenderer::BLUE = "\033[34m";
const std::string TerminalMarkdownRenderer::MAGENTA = "\033[35m";
const std::string TerminalMarkdownRenderer::CYAN = "\033[36m";
const std::string TerminalMarkdownRenderer::WHITE = "\033[37m";

const std::string TerminalMarkdownRenderer::BG_BLACK = "\033[40m";
const std::string TerminalMarkdownRenderer::BG_GRAY = "\033[100m";

/**
 * Renders markdown text to ANSI-formatted terminal output.
 */
std::string TerminalMarkdownRenderer::render(const std::string& markdown) {
    RenderState state;
    
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
    
    // Parse the markdown
    int result = md_parse(markdown.c_str(), markdown.size(), &parser, &state);
    
    if (result != 0) {
        // Fallback to plain text if parsing fails
        return markdown + "\n";
    }
    
    return state.output;
}

/**
 * Callback for entering block elements (headers, paragraphs, code blocks, etc.)
 */
int TerminalMarkdownRenderer::enterBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata) {
    auto* state = static_cast<RenderState*>(userdata);
    
    switch (blockType) {
        case MD_BLOCK_H: {
            auto* headerDetail = static_cast<MD_BLOCK_H_DETAIL*>(detail);
            state->output += getHeaderColor(headerDetail->level) + BOLD;
            break;
        }
        case MD_BLOCK_P:
            addIndentation(*state);
            break;
        case MD_BLOCK_CODE: {
            auto* codeDetail = static_cast<MD_BLOCK_CODE_DETAIL*>(detail);
            state->inCodeBlock = true;
            state->output += "\n" + BG_GRAY + BLACK;
            addIndentation(*state);
            if (codeDetail->lang.text != nullptr) {
                std::string lang(codeDetail->lang.text, codeDetail->lang.size);
                state->output += DIM + "(" + lang + ")" + RESET + BG_GRAY + BLACK + "\n";
                addIndentation(*state);
            }
            break;
        }
        case MD_BLOCK_UL:
        case MD_BLOCK_OL:
            state->inList = true;
            state->listLevel++;
            break;
        case MD_BLOCK_LI:
            state->output += "\n";
            addIndentation(*state);
            if (state->inList) {
                for (int i = 0; i < state->listLevel - 1; i++) {
                    state->output += "  ";
                }
                state->output += YELLOW + "• " + RESET;
            }
            break;
        case MD_BLOCK_QUOTE:
            state->output += "\n";
            addIndentation(*state);
            state->output += DIM + "│ " + RESET;
            state->indentLevel++;
            break;
        case MD_BLOCK_HR:
            state->output += "\n" + DIM + "────────────────────────────────────────" + RESET + "\n";
            break;
        case MD_BLOCK_TABLE:
            state->output += "\n";
            break;
        case MD_BLOCK_TR:
            addIndentation(*state);
            state->output += DIM + "│" + RESET;
            break;
        case MD_BLOCK_TD:
        case MD_BLOCK_TH:
            state->output += " ";
            break;
    }
    
    return 0;
}

/**
 * Callback for leaving block elements
 */
int TerminalMarkdownRenderer::leaveBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata) {
    auto* state = static_cast<RenderState*>(userdata);
    
    switch (blockType) {
        case MD_BLOCK_H:
            state->output += RESET + "\n\n";
            break;
        case MD_BLOCK_P:
            state->output += "\n\n";
            break;
        case MD_BLOCK_CODE:
            state->inCodeBlock = false;
            state->output += RESET + "\n\n";
            break;
        case MD_BLOCK_UL:
        case MD_BLOCK_OL:
            state->inList = false;
            state->listLevel--;
            state->output += "\n";
            break;
        case MD_BLOCK_LI:
            break;
        case MD_BLOCK_QUOTE:
            state->indentLevel--;
            state->output += "\n";
            break;
        case MD_BLOCK_TABLE:
            state->output += "\n";
            break;
        case MD_BLOCK_TR:
            state->output += DIM + " │" + RESET + "\n";
            break;
        case MD_BLOCK_TD:
        case MD_BLOCK_TH:
            state->output += DIM + " │" + RESET;
            break;
    }
    
    return 0;
}

/**
 * Callback for entering span elements (emphasis, links, code, etc.)
 */
int TerminalMarkdownRenderer::enterSpanCallback(MD_SPANTYPE spanType, void* detail, void* userdata) {
    auto* state = static_cast<RenderState*>(userdata);
    
    switch (spanType) {
        case MD_SPAN_EM:
            state->output += DIM;
            break;
        case MD_SPAN_STRONG:
            state->output += BOLD;
            break;
        case MD_SPAN_CODE:
            state->output += BG_GRAY + BLACK;
            break;
        case MD_SPAN_A: {
            auto* linkDetail = static_cast<MD_SPAN_A_DETAIL*>(detail);
            state->output += BLUE + UNDERLINE;
            break;
        }
        case MD_SPAN_DEL:
            state->output += DIM;
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
            if (state->inCodeBlock) {
                state->output += RESET + BG_GRAY + BLACK;
            } else {
                state->output += RESET;
            }
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
            state->output += escapeAnsiSequences(textStr);
            break;
        case MD_TEXT_NULLCHAR:
            // Skip null characters
            break;
        case MD_TEXT_BR:
        case MD_TEXT_SOFTBR:
            state->output += "\n";
            if (state->inCodeBlock) {
                addIndentation(*state);
            }
            break;
    }
    
    return 0;
}

/**
 * Adds appropriate indentation based on current state
 */
void TerminalMarkdownRenderer::addIndentation(RenderState& state) {
    for (int i = 0; i < state.indentLevel; i++) {
        state.output += "  ";
    }
}

/**
 * Returns appropriate color for header level
 */
std::string TerminalMarkdownRenderer::getHeaderColor(int level) {
    switch (level) {
        case 1: return BLUE;
        case 2: return GREEN;
        case 3: return YELLOW;
        case 4: return MAGENTA;
        case 5: return CYAN;
        case 6: return WHITE;
        default: return WHITE;
    }
}

/**
 * Escapes any existing ANSI sequences in text to prevent conflicts
 */
std::string TerminalMarkdownRenderer::escapeAnsiSequences(const std::string& text) {
    // For now, just return the text as-is
    // In a more robust implementation, we might escape existing ANSI codes
    return text;
}
