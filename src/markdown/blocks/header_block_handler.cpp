#include "markdown/blocks/header_block_handler.h"
#include "markdown/common/ansi_colors.h"
#include <md4c.h>

namespace markdown {

bool HeaderBlockHandler::canHandle(MD_BLOCKTYPE blockType) const {
    return blockType == MD_BLOCK_H;
}

int HeaderBlockHandler::enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    auto* headerDetail = static_cast<MD_BLOCK_H_DETAIL*>(detail);
    state.output += AnsiColors::getHeaderColor(headerDetail->level) + AnsiColors::BOLD;
    
    return 0;
}

int HeaderBlockHandler::leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    state.output += AnsiColors::RESET;
    
    return 0;
}

} // namespace markdown
