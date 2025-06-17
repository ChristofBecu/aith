#pragma once

#include "markdown/blocks/block_handler.h"

namespace markdown {

/**
 * @brief Handles markdown paragraph blocks
 * 
 * Manages paragraph formatting with proper indentation and blockquote support.
 */
class ParagraphBlockHandler : public BlockHandler {
public:
    bool canHandle(MD_BLOCKTYPE blockType) const override;
    
    int enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;
    int leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;

private:
    void addBlockquotePrefixes(RenderState& state) const;
    void addIndentation(RenderState& state) const;
};

} // namespace markdown
