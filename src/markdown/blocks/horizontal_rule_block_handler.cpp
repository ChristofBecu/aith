#include "markdown/blocks/horizontal_rule_block_handler.h"
#include "markdown/common/ansi_colors.h"
#include <md4c.h>

namespace markdown {

bool HorizontalRuleBlockHandler::canHandle(MD_BLOCKTYPE blockType) const {
    return blockType == MD_BLOCK_HR;
}

int HorizontalRuleBlockHandler::enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    //state.output += "\n";
    addIndentation(state);
    //state.output += AnsiColors::DIM + AnsiColors::WHITE + "────────────────────────────────────────" + AnsiColors::RESET + "\n";
    state.output += AnsiColors::DIM + AnsiColors::WHITE + "────────────────────────────────────────" + AnsiColors::RESET;
    return 0;
}

int HorizontalRuleBlockHandler::leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    // No specific action needed for horizontal rule end
    return 0;
}

void HorizontalRuleBlockHandler::addIndentation(RenderState& state) const {
    for (int i = 0; i < state.indentLevel; i++) {
        state.output += "  ";
    }
}

} // namespace markdown
