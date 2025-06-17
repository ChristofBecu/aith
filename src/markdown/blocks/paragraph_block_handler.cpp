#include "markdown/blocks/paragraph_block_handler.h"
#include "markdown/common/ansi_colors.h"
#include <md4c.h>

namespace markdown {

bool ParagraphBlockHandler::canHandle(MD_BLOCKTYPE blockType) const {
    return blockType == MD_BLOCK_P;
}

int ParagraphBlockHandler::enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    addBlockquotePrefixes(state);
    addIndentation(state);
    
    return 0;
}

int ParagraphBlockHandler::leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    // No trailing spacing - handled by BlockHandlerFactory
    
    return 0;
}

void ParagraphBlockHandler::addBlockquotePrefixes(RenderState& state) const {
    for (int i = 0; i < state.blockquoteLevel; i++) {
        state.output += AnsiColors::DIM + AnsiColors::WHITE + "│ " + AnsiColors::RESET;
    }
}

void ParagraphBlockHandler::addIndentation(RenderState& state) const {
    for (int i = 0; i < state.indentLevel; i++) {
        state.output += "  ";
    }
}

} // namespace markdown
