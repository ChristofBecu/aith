#include "markdown/blocks/blockquote_block_handler.h"
#include <md4c.h>

namespace markdown {

bool BlockquoteBlockHandler::canHandle(MD_BLOCKTYPE blockType) const {
    return blockType == MD_BLOCK_QUOTE;
}

int BlockquoteBlockHandler::enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    //state.output += "\n";
    state.blockquoteLevel++;
    
    return 0;
}

int BlockquoteBlockHandler::leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    state.blockquoteLevel--;
    // Trailing spacing now handled by BlockHandlerFactory
    
    return 0;
}

} // namespace markdown
