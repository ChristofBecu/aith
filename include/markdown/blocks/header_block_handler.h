#pragma once

#include "markdown/blocks/block_handler.h"

namespace markdown {

/**
 * @brief Handles markdown header blocks (H1-H6)
 * 
 * Provides hierarchical white/yellow/green/cyan colors with bold formatting
 * for different header levels.
 */
class HeaderBlockHandler : public BlockHandler {
public:
    bool canHandle(MD_BLOCKTYPE blockType) const override;
    
    int enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;
    int leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;
};

} // namespace markdown
