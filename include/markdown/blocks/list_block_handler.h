#pragma once

#include "markdown/blocks/block_handler.h"

namespace markdown {

/**
 * @brief Handles markdown list blocks (UL, OL, LI)
 * 
 * Manages both unordered and ordered lists with proper nesting support.
 */
class ListBlockHandler : public BlockHandler {
public:
    bool canHandle(MD_BLOCKTYPE blockType) const override;
    
    int enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;
    int leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;

private:
    void addIndentation(RenderState& state) const;
    void handleListItem(RenderState& state) const;
};

} // namespace markdown
