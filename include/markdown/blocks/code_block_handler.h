#pragma once

#include "markdown/blocks/block_handler.h"

namespace markdown {

/**
 * @brief Handles markdown code blocks
 * 
 * Provides syntax highlighting hints and proper formatting for code blocks.
 */
class CodeBlockHandler : public BlockHandler {
public:
    bool canHandle(MD_BLOCKTYPE blockType) const override;
    
    int enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;
    int leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;

private:
    void addIndentation(RenderState& state) const;
};

} // namespace markdown
