#ifndef MARKDOWN_RENDERER_H
#define MARKDOWN_RENDERER_H

#include <string>
#include <vector>
#include <memory>
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
    struct TableState {
        std::vector<std::vector<std::string>> rows;
        std::vector<size_t> columnWidths;
        std::vector<std::string> currentRow;
        std::string currentCellContent;
        bool isHeader;
        size_t currentCol;
        
        TableState() : isHeader(false), currentCol(0) {}
        
        void startNewRow() {
            if (!currentRow.empty()) {
                rows.push_back(currentRow);
                currentRow.clear();
            }
            currentCol = 0;
        }
        
        void addCell(const std::string& content) {
            currentRow.push_back(content);
            currentCol++;
        }
        
        void finalize() {
            if (!currentRow.empty()) {
                rows.push_back(currentRow);
            }
        }
    };

    struct RenderState {
        std::string output;
        int indentLevel;
        bool inCodeBlock;
        bool inList;
        int listLevel;
        std::vector<bool> isOrderedList;  // Track if each list level is ordered
        std::vector<int> listItemCount;   // Track current item number for ordered lists
        bool inTableHeader;               // Track if we're in a table header
        int blockquoteLevel;              // Track blockquote nesting level
        std::unique_ptr<TableState> currentTable;  // Current table being processed
        
        RenderState() : indentLevel(0), inCodeBlock(false), inList(false), 
                       listLevel(0), inTableHeader(false), blockquoteLevel(0) {}
    };
    
    // Static callback functions for md4c parser
    static int enterBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata);
    static int leaveBlockCallback(MD_BLOCKTYPE blockType, void* detail, void* userdata);
    static int enterSpanCallback(MD_SPANTYPE spanType, void* detail, void* userdata);
    static int leaveSpanCallback(MD_SPANTYPE spanType, void* detail, void* userdata);
    static int textCallback(MD_TEXTTYPE textType, const MD_CHAR* text, MD_SIZE size, void* userdata);
    
    // Static helper methods for formatting (since callbacks are static)
    static void addIndentation(RenderState& state);
    static void addBlockquotePrefixes(RenderState& state);
    static std::string getHeaderColor(int level);
    static std::string escapeAnsiSequences(const std::string& text);
    static std::string decodeJsonAndUnicodeEscapes(const std::string& text);
    
    // Table rendering helpers
    static size_t getDisplayWidth(const std::string& text);
    static void calculateColumnWidths(TableState& table);
    static void renderTable(RenderState& state);
    static std::string padCell(const std::string& content, size_t width, bool isHeader = false);
};

#endif // MARKDOWN_RENDERER_H
