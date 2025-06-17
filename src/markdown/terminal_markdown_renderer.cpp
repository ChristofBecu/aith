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
    // First decode JSON escapes and Unicode HTML entities
    std::string decodedMarkdown = decodeJsonAndUnicodeEscapes(markdown);
    
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
            state->output += getHeaderColor(headerDetail->level) + BOLD;
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
                state->output += GREEN + "(" + lang + ")" + RESET + "\n";
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
                    state->output += DIM + WHITE + std::to_string(itemNumber) + ". " + RESET;
                    state->listItemCount[currentLevel]++;
                } else {
                    // Unordered list - show bullet
                    state->output += DIM + WHITE + "• " + RESET;
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
            state->output += DIM + WHITE + "────────────────────────────────────────" + RESET + "\n";
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
            state->output += DIM;
            break;
        case MD_SPAN_STRONG:
            state->output += BOLD;
            break;
        case MD_SPAN_CODE:
            state->output += DIM + CYAN;
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
            state->output += RESET;
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
                state->output += escapeAnsiSequences(textStr);
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
        state.output += DIM + WHITE + "│ " + RESET;
    }
}

/**
 * Returns appropriate color for header level with hierarchical intensity
 * H1 (brightest) -> H6 (dimmest) for clear visual hierarchy
 */
std::string TerminalMarkdownRenderer::getHeaderColor(int level) {
    switch (level) {
        case 1: return WHITE;             // H1: Bright white + bold (from caller)
        case 2: return WHITE;             // H2: White + bold (from caller)
        case 3: return YELLOW;            // H3: Yellow + bold (from caller)
        case 4: return GREEN;             // H4: Green + bold (from caller)
        case 5: return CYAN;              // H5: Cyan + bold (from caller)
        case 6: return DIM + WHITE;       // H6: Dimmed white + bold (from caller)
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

/**
 * Decodes JSON escape sequences and Unicode HTML entities in the input text.
 * This handles strings that come from JSON-encoded markdown content.
 */
std::string TerminalMarkdownRenderer::decodeJsonAndUnicodeEscapes(const std::string& text) {
    std::string result;
    result.reserve(text.length());
    
    for (size_t i = 0; i < text.length(); ++i) {
        if (text[i] == '\\' && i + 1 < text.length()) {
            char next = text[i + 1];
            switch (next) {
                case 'n':
                    result += '\n';
                    ++i; // Skip the next character
                    break;
                case 't':
                    result += '\t';
                    ++i;
                    break;
                case 'r':
                    result += '\r';
                    ++i;
                    break;
                case '\\':
                    result += '\\';
                    ++i;
                    break;
                case '"':
                    result += '"';
                    ++i;
                    break;
                case '/':
                    result += '/';
                    ++i;
                    break;
                case 'u':
                    // Handle Unicode escape sequences like \u003c, \u003e
                    if (i + 5 < text.length()) {
                        std::string hexCode = text.substr(i + 2, 4);
                        try {
                            // Convert hex string to integer
                            unsigned int codePoint = std::stoul(hexCode, nullptr, 16);
                            
                            // Convert common HTML entities to their ASCII equivalents
                            switch (codePoint) {
                                case 0x003c: // \u003c -> <
                                    result += '<';
                                    break;
                                case 0x003e: // \u003e -> >
                                    result += '>';
                                    break;
                                case 0x0026: // \u0026 -> &
                                    result += '&';
                                    break;
                                case 0x0022: // \u0022 -> "
                                    result += '"';
                                    break;
                                case 0x0027: // \u0027 -> '
                                    result += '\'';
                                    break;
                                default:
                                    // For other Unicode characters, convert to UTF-8
                                    if (codePoint <= 0x7F) {
                                        // ASCII range
                                        result += static_cast<char>(codePoint);
                                    } else if (codePoint <= 0x7FF) {
                                        // 2-byte UTF-8
                                        result += static_cast<char>(0xC0 | (codePoint >> 6));
                                        result += static_cast<char>(0x80 | (codePoint & 0x3F));
                                    } else {
                                        // 3-byte UTF-8 (sufficient for \uXXXX range)
                                        result += static_cast<char>(0xE0 | (codePoint >> 12));
                                        result += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
                                        result += static_cast<char>(0x80 | (codePoint & 0x3F));
                                    }
                                    break;
                            }
                            i += 5; // Skip \uXXXX
                        } catch (const std::exception&) {
                            // If parsing fails, keep the original sequence
                            result += text[i];
                        }
                    } else {
                        // Not enough characters for a valid \uXXXX sequence
                        result += text[i];
                    }
                    break;
                default:
                    // Unknown escape sequence, keep both characters
                    result += text[i];
                    result += next;
                    ++i;
                    break;
            }
        } else {
            result += text[i];
        }
    }
    
    return result;
}

/**
 * Gets the display width of text (handles basic UTF-8)
 */
size_t TerminalMarkdownRenderer::getDisplayWidth(const std::string& text) {
    // Simple implementation - count characters, not bytes
    // This is a basic version; a full implementation would handle Unicode properly
    size_t width = 0;
    for (size_t i = 0; i < text.length(); ) {
        unsigned char c = text[i];
        if (c < 0x80) {
            // ASCII character
            width++;
            i++;
        } else if ((c & 0xE0) == 0xC0) {
            // 2-byte UTF-8
            width++;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            // 3-byte UTF-8
            width++;
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            // 4-byte UTF-8
            width++;
            i += 4;
        } else {
            // Invalid UTF-8, skip
            i++;
        }
    }
    return width;
}

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
            size_t cellWidth = getDisplayWidth(row[col]);
            table.columnWidths[col] = std::max(table.columnWidths[col], cellWidth);
        }
    }
    
    // Add padding (2 spaces per side)
    for (auto& width : table.columnWidths) {
        width += 2;
    }
}

/**
 * Pads cell content to specified width
 */
std::string TerminalMarkdownRenderer::padCell(const std::string& content, size_t width, bool isHeader) {
    size_t contentWidth = getDisplayWidth(content);
    size_t padding = (width > contentWidth) ? width - contentWidth : 0;
    
    std::string result = " " + content;
    for (size_t i = 0; i < padding - 1; i++) {
        result += " ";
    }
    
    if (isHeader) {
        return BOLD + WHITE + result + RESET;
    } else {
        return result;
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
    state.output += DIM + WHITE + "┌";
    for (size_t col = 0; col < table.columnWidths.size(); col++) {
        for (size_t i = 0; i < table.columnWidths[col]; i++) {
            state.output += "─";
        }
        if (col < table.columnWidths.size() - 1) {
            state.output += "┬";
        }
    }
    state.output += "┐" + RESET + "\n";
    
    // Render rows
    bool isFirstRow = true;
    for (const auto& row : table.rows) {
        // Row content
        addBlockquotePrefixes(state);
        addIndentation(state);
        state.output += DIM + WHITE + "│" + RESET;
        
        for (size_t col = 0; col < table.columnWidths.size(); col++) {
            std::string cellContent = (col < row.size()) ? row[col] : "";
            state.output += padCell(cellContent, table.columnWidths[col], isFirstRow);
            state.output += DIM + WHITE + "│" + RESET;
        }
        state.output += "\n";
        
        // Separator after header row
        if (isFirstRow && table.rows.size() > 1) {
            addBlockquotePrefixes(state);
            addIndentation(state);
            state.output += DIM + WHITE + "├";
            for (size_t col = 0; col < table.columnWidths.size(); col++) {
                for (size_t i = 0; i < table.columnWidths[col]; i++) {
                    state.output += "─";
                }
                if (col < table.columnWidths.size() - 1) {
                    state.output += "┼";
                }
            }
            state.output += "┤" + RESET + "\n";
        }
        
        isFirstRow = false;
    }
    
    // Bottom border
    addBlockquotePrefixes(state);
    addIndentation(state);
    state.output += DIM + WHITE + "└";
    for (size_t col = 0; col < table.columnWidths.size(); col++) {
        for (size_t i = 0; i < table.columnWidths[col]; i++) {
            state.output += "─";
        }
        if (col < table.columnWidths.size() - 1) {
            state.output += "┴";
        }
    }
    state.output += "┘" + RESET + "\n";
}
