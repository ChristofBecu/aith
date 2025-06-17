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
 * Renders markdown text to ANSI-formatted terminal output.
 */
std::string TerminalMarkdownRenderer::render(const std::string& markdown) {
    // First decode JSON escapes and Unicode HTML entities
    std::string decodedMarkdown = TextUtils::decodeJsonAndUnicodeEscapes(markdown);
    
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
    
    switch (blockType) {
        case MD_BLOCK_H: {
            auto* headerDetail = static_cast<MD_BLOCK_H_DETAIL*>(detail);
            state->output += AnsiColors::getHeaderColor(headerDetail->level) + AnsiColors::BOLD;
            break;
        }
        case MD_BLOCK_P:
            addBlockquotePrefixes(*state);
            addIndentation(*state);
            break;
        case MD_BLOCK_CODE: {
            auto* codeDetail = static_cast<MD_BLOCK_CODE_DETAIL*>(detail);
            state->inCodeBlock = true;
            state->output += "\n";
            addIndentation(*state);
            if (codeDetail->lang.text != nullptr) {
                std::string lang(codeDetail->lang.text, codeDetail->lang.size);
                state->output += AnsiColors::GREEN + "(" + lang + ")" + AnsiColors::RESET + "\n";
                addIndentation(*state);
            }
            break;
        }
        case MD_BLOCK_UL:
            state->inList = true;
            state->listLevel++;
            state->isOrderedList.resize(state->listLevel, false);
            state->listItemCount.resize(state->listLevel, 0);
            state->isOrderedList[state->listLevel - 1] = false;
            break;
        case MD_BLOCK_OL: {
            auto* olDetail = static_cast<MD_BLOCK_OL_DETAIL*>(detail);
            state->inList = true;
            state->listLevel++;
            state->isOrderedList.resize(state->listLevel, false);
            state->listItemCount.resize(state->listLevel, 0);
            state->isOrderedList[state->listLevel - 1] = true;
            state->listItemCount[state->listLevel - 1] = olDetail ? olDetail->start : 1;
            break;
        }
        case MD_BLOCK_LI: {
            state->output += "\n";
            addIndentation(*state);
            for (int i = 0; i < state->listLevel - 1; i++) {
                state->output += "  ";
            }
            
            if (state->inList && state->listLevel > 0) {
                int currentLevel = state->listLevel - 1;
                if (currentLevel < static_cast<int>(state->isOrderedList.size()) && 
                    state->isOrderedList[currentLevel]) {
                    // Ordered list - show number
                    int itemNumber = state->listItemCount[currentLevel];
                    state->output += AnsiColors::DIM + AnsiColors::WHITE + std::to_string(itemNumber) + ". " + AnsiColors::RESET;
                    state->listItemCount[currentLevel]++;
                } else {
                    // Unordered list - show bullet
                    state->output += AnsiColors::DIM + AnsiColors::WHITE + "• " + AnsiColors::RESET;
                }
            }
            break;
        }
        case MD_BLOCK_QUOTE:
            state->output += "\n";
            state->blockquoteLevel++;
            break;
        case MD_BLOCK_HR:
            state->output += "\n";
            addIndentation(*state);
            state->output += AnsiColors::DIM + AnsiColors::WHITE + "────────────────────────────────────────" + AnsiColors::RESET + "\n";
            break;
        case MD_BLOCK_TABLE:
            state->output += "\n";
            state->currentTable = std::make_unique<TableState>();
            break;
        case MD_BLOCK_THEAD:
            if (state->currentTable) {
                state->currentTable->isHeader = true;
            }
            break;
        case MD_BLOCK_TBODY:
            if (state->currentTable) {
                state->currentTable->isHeader = false;
            }
            break;
        case MD_BLOCK_TR:
            if (state->currentTable) {
                state->currentTable->startNewRow();
            }
            break;
        case MD_BLOCK_TD:
        case MD_BLOCK_TH:
            if (state->currentTable) {
                state->currentTable->currentCellContent.clear();
            }
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
            state->output += AnsiColors::RESET + "\n\n";
            break;
        case MD_BLOCK_P:
            state->output += "\n\n";
            break;
        case MD_BLOCK_CODE:
            state->inCodeBlock = false;
            state->output += AnsiColors::RESET + "\n\n";
            break;
        case MD_BLOCK_UL:
        case MD_BLOCK_OL:
            state->inList = (state->listLevel > 1);
            state->listLevel--;
            if (static_cast<int>(state->isOrderedList.size()) > state->listLevel) {
                state->isOrderedList.resize(state->listLevel);
            }
            if (static_cast<int>(state->listItemCount.size()) > state->listLevel) {
                state->listItemCount.resize(state->listLevel);
            }
            // Add proper spacing after list ends (two newlines for consistent paragraph separation)
            state->output += "\n\n";
            break;
        case MD_BLOCK_LI:
            break;
        case MD_BLOCK_QUOTE:
            state->blockquoteLevel--;
            state->output += "\n";
            break;
        case MD_BLOCK_TABLE:
            if (state->currentTable) {
                state->currentTable->finalize();
                calculateColumnWidths(*state->currentTable);
                renderTable(*state);
                state->currentTable.reset();
            }
            state->output += "\n";
            break;
        case MD_BLOCK_THEAD:
        case MD_BLOCK_TBODY:
            // No specific action needed
            break;
        case MD_BLOCK_TR:
            // Row end handled in table state
            break;
        case MD_BLOCK_TD:
        case MD_BLOCK_TH:
            if (state->currentTable) {
                state->currentTable->addCell(state->currentTable->currentCellContent);
            }
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

/**
 * Escapes any existing ANSI sequences in text to prevent conflicts
 */


/**
 * Gets the display width of text (handles basic UTF-8)
 */
/**
 * Calculates optimal column widths for the table
 */
void TerminalMarkdownRenderer::calculateColumnWidths(TableState& table) {
    if (table.rows.empty()) return;
    
    // Determine number of columns
    size_t maxCols = 0;
    for (const auto& row : table.rows) {
        maxCols = std::max(maxCols, row.size());
    }
    
    table.columnWidths.resize(maxCols, 0);
    
    // Find maximum width for each column
    for (const auto& row : table.rows) {
        for (size_t col = 0; col < row.size() && col < maxCols; col++) {
            size_t cellWidth = TextUtils::getDisplayWidth(row[col]);
            table.columnWidths[col] = std::max(table.columnWidths[col], cellWidth);
        }
    }
    
    // Add padding (2 spaces per side)
    for (auto& width : table.columnWidths) {
        width += 2;
    }
}

/**
 * Renders the complete table with box drawing characters
 */
void TerminalMarkdownRenderer::renderTable(RenderState& state) {
    if (!state.currentTable || state.currentTable->rows.empty()) return;
    
    TableState& table = *state.currentTable;
    
    addBlockquotePrefixes(state);
    addIndentation(state);
    
    // Top border
    state.output += AnsiColors::DIM + AnsiColors::WHITE + "┌";
    for (size_t col = 0; col < table.columnWidths.size(); col++) {
        for (size_t i = 0; i < table.columnWidths[col]; i++) {
            state.output += "─";
        }
        if (col < table.columnWidths.size() - 1) {
            state.output += "┬";
        }
    }
    state.output += "┐" + AnsiColors::RESET + "\n";
    
    // Render rows
    bool isFirstRow = true;
    for (const auto& row : table.rows) {
        // Row content
        addBlockquotePrefixes(state);
        addIndentation(state);
        state.output += AnsiColors::DIM + AnsiColors::WHITE + "│" + AnsiColors::RESET;
        
        for (size_t col = 0; col < table.columnWidths.size(); col++) {
            std::string cellContent = (col < row.size()) ? row[col] : "";
            state.output += TextUtils::padCell(cellContent, table.columnWidths[col], isFirstRow);
            state.output += AnsiColors::DIM + AnsiColors::WHITE + "│" + AnsiColors::RESET;
        }
        state.output += "\n";
        
        // Separator after header row
        if (isFirstRow && table.rows.size() > 1) {
            addBlockquotePrefixes(state);
            addIndentation(state);
            state.output += AnsiColors::DIM + AnsiColors::WHITE + "├";
            for (size_t col = 0; col < table.columnWidths.size(); col++) {
                for (size_t i = 0; i < table.columnWidths[col]; i++) {
                    state.output += "─";
                }
                if (col < table.columnWidths.size() - 1) {
                    state.output += "┼";
                }
            }
            state.output += "┤" + AnsiColors::RESET + "\n";
        }
        
        isFirstRow = false;
    }
    
    // Bottom border
    addBlockquotePrefixes(state);
    addIndentation(state);
    state.output += AnsiColors::DIM + AnsiColors::WHITE + "└";
    for (size_t col = 0; col < table.columnWidths.size(); col++) {
        for (size_t i = 0; i < table.columnWidths[col]; i++) {
            state.output += "─";
        }
        if (col < table.columnWidths.size() - 1) {
            state.output += "┴";
        }
    }
    state.output += "┘" + AnsiColors::RESET + "\n";
}
