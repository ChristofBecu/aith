#ifndef MARKDOWN_RENDER_STATE_H
#define MARKDOWN_RENDER_STATE_H

#include <string>
#include <vector>
#include <memory>

namespace markdown {

/**
 * @brief State for tracking table rendering and content.
 * 
 * This structure manages table content during the parsing and rendering process,
 * including row/column tracking, content buffering, and finalization.
 */
struct TableState {
    std::vector<std::vector<std::string>> rows;
    std::vector<size_t> columnWidths;
    std::vector<std::string> currentRow;
    std::string currentCellContent;
    bool isHeader;
    size_t currentCol;
    
    TableState() : isHeader(false), currentCol(0) {}
    
    /**
     * @brief Starts a new table row.
     * 
     * Finalizes the current row and prepares for the next one.
     */
    void startNewRow() {
        if (!currentRow.empty()) {
            rows.push_back(currentRow);
            currentRow.clear();
        }
        currentCol = 0;
    }
    
    /**
     * @brief Adds a cell to the current row.
     * 
     * @param content The cell content to add
     */
    void addCell(const std::string& content) {
        currentRow.push_back(content);
        currentCol++;
    }
    
    /**
     * @brief Finalizes the table by adding any remaining row.
     */
    void finalize() {
        if (!currentRow.empty()) {
            rows.push_back(currentRow);
        }
    }
};

/**
 * @brief Main rendering state for tracking markdown processing.
 * 
 * This structure maintains all state information needed during markdown
 * parsing and rendering, including output buffer, nesting levels, and
 * current rendering context.
 */
struct RenderState {
    std::string output;                              // Accumulated output
    int indentLevel;                                 // Current indentation level
    bool inCodeBlock;                               // Whether we're in a code block
    bool inList;                                    // Whether we're in a list
    int listLevel;                                  // Current list nesting level
    std::vector<bool> isOrderedList;               // Track if each list level is ordered
    std::vector<int> listItemCount;                // Track current item number for ordered lists
    bool inTableHeader;                            // Track if we're in a table header
    int blockquoteLevel;                           // Track blockquote nesting level
    std::unique_ptr<TableState> currentTable;     // Current table being processed
    
    RenderState() : indentLevel(0), inCodeBlock(false), inList(false), 
                   listLevel(0), inTableHeader(false), blockquoteLevel(0) {}
};

} // namespace markdown

#endif // MARKDOWN_RENDER_STATE_H
