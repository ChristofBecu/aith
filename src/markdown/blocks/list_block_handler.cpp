#include "markdown/blocks/list_block_handler.h"
#include "markdown/common/ansi_colors.h"
#include <md4c.h>
#include <string>

namespace markdown {

bool ListBlockHandler::canHandle(MD_BLOCKTYPE blockType) const {
    return blockType == MD_BLOCK_UL || blockType == MD_BLOCK_OL || blockType == MD_BLOCK_LI;
}

int ListBlockHandler::enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    switch (blockType) {
        case MD_BLOCK_UL:
            state.inList = true;
            state.listLevel++;
            state.isOrderedList.resize(state.listLevel, false);
            state.listItemCount.resize(state.listLevel, 0);
            state.isOrderedList[state.listLevel - 1] = false;
            break;
            
        case MD_BLOCK_OL: {
            auto* olDetail = static_cast<MD_BLOCK_OL_DETAIL*>(detail);
            state.inList = true;
            state.listLevel++;
            state.isOrderedList.resize(state.listLevel, false);
            state.listItemCount.resize(state.listLevel, 0);
            state.isOrderedList[state.listLevel - 1] = true;
            state.listItemCount[state.listLevel - 1] = olDetail ? olDetail->start : 1;
            break;
        }
        
        case MD_BLOCK_LI:
            handleListItem(state);
            break;
    }
    
    return 0;
}

int ListBlockHandler::leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    switch (blockType) {
        case MD_BLOCK_UL:
        case MD_BLOCK_OL:
            state.inList = (state.listLevel > 1);
            state.listLevel--;
            if (static_cast<int>(state.isOrderedList.size()) > state.listLevel) {
                state.isOrderedList.resize(state.listLevel);
            }
            if (static_cast<int>(state.listItemCount.size()) > state.listLevel) {
                state.listItemCount.resize(state.listLevel);
            }
            // Trailing spacing now handled by BlockHandlerFactory
            break;
            
        case MD_BLOCK_LI:
            // No specific action needed for list item end
            // add trailing spacing if needed
            if (state.listLevel != 0) {
                // If we're still in a list, add a single newline
                state.output += "\n";
            }
                // state.output += "\n"; // End of outermost list gets double newlines
            break;
    }
    
    return 0;
}

void ListBlockHandler::addIndentation(RenderState& state) const {
    for (int i = 0; i < state.indentLevel; i++) {
        state.output += "  ";
    }
}

void ListBlockHandler::handleListItem(RenderState& state) const {
    // state.output += "\n";
    addIndentation(state);
    
    // Add list nesting indentation
    for (int i = 0; i < state.listLevel - 1; i++) {
        state.output += "  ";
    }
    
    if (state.inList && state.listLevel > 0) {
        int currentLevel = state.listLevel - 1;
        if (currentLevel < static_cast<int>(state.isOrderedList.size()) && 
            state.isOrderedList[currentLevel]) {
            // Ordered list - show number
            int itemNumber = state.listItemCount[currentLevel];
            state.output += AnsiColors::DIM + AnsiColors::WHITE + std::to_string(itemNumber) + ". " + AnsiColors::RESET;
            state.listItemCount[currentLevel]++;
        } else {
            // Unordered list - show bullet
            state.output += AnsiColors::DIM + AnsiColors::WHITE + "• " + AnsiColors::RESET;
        }

    }
}

} // namespace markdown
