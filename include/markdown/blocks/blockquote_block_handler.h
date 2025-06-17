#pragma once

#include "markdown/blocks/block_handler.h"

namespace markdown {

/**
 * @brief Handles markdown blockquote blocks
 * 
 * Provides vertical bar prefix (│) with proper multi-line support and nesting.
 */
class BlockquoteBlockHandler : public BlockHandler {
public:
    bool canHandle(MD_BLOCKTYPE blockType) const override;
    
    int enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;
    int leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;
};

} // namespace markdown
