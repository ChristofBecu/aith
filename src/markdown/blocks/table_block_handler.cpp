#include "markdown/blocks/table_block_handler.h"
#include "markdown/common/ansi_colors.h"
#include "markdown/common/text_utils.h"
#include <md4c.h>
#include <algorithm>
#include <memory>

namespace markdown {

bool TableBlockHandler::canHandle(MD_BLOCKTYPE blockType) const {
    return blockType == MD_BLOCK_TABLE ||
           blockType == MD_BLOCK_THEAD ||
           blockType == MD_BLOCK_TBODY ||
           blockType == MD_BLOCK_TR ||
           blockType == MD_BLOCK_TD ||
           blockType == MD_BLOCK_TH;
}

int TableBlockHandler::enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    switch (blockType) {
        case MD_BLOCK_TABLE:
            state.output += "\n";
            state.currentTable = std::make_unique<TableState>();
            break;
            
        case MD_BLOCK_THEAD:
            if (state.currentTable) {
                state.currentTable->isHeader = true;
            }
            break;
            
        case MD_BLOCK_TBODY:
            if (state.currentTable) {
                state.currentTable->isHeader = false;
            }
            break;
            
        case MD_BLOCK_TR:
            if (state.currentTable) {
                state.currentTable->startNewRow();
            }
            break;
            
        case MD_BLOCK_TD:
        case MD_BLOCK_TH:
            if (state.currentTable) {
                state.currentTable->currentCellContent.clear();
            }
            break;
    }
    
    return 0;
}

int TableBlockHandler::leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    switch (blockType) {
        case MD_BLOCK_TABLE:
            if (state.currentTable) {
                state.currentTable->finalize();
                calculateColumnWidths(*state.currentTable);
                renderTable(state);
                state.currentTable.reset();
            }
            state.output += "\n";
            break;
            
        case MD_BLOCK_THEAD:
        case MD_BLOCK_TBODY:
        case MD_BLOCK_TR:
            // No specific action needed
            break;
            
        case MD_BLOCK_TD:
        case MD_BLOCK_TH:
            if (state.currentTable) {
                state.currentTable->addCell(state.currentTable->currentCellContent);
            }
            break;
    }
    
    return 0;
}

void TableBlockHandler::calculateColumnWidths(TableState& table) const {
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

void TableBlockHandler::renderTable(RenderState& state) const {
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

void TableBlockHandler::addBlockquotePrefixes(RenderState& state) const {
    for (int i = 0; i < state.blockquoteLevel; i++) {
        state.output += AnsiColors::DIM + AnsiColors::WHITE + "│ " + AnsiColors::RESET;
    }
}

void TableBlockHandler::addIndentation(RenderState& state) const {
    for (int i = 0; i < state.indentLevel; i++) {
        state.output += "  ";
    }
}

} // namespace markdown
