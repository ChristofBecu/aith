#pragma once

#include "markdown/blocks/block_handler.h"

namespace markdown {

/**
 * @brief Handles markdown table blocks (TABLE, THEAD, TBODY, TR, TD, TH)
 * 
 * Provides professional table formatting with box-drawing characters and proper column sizing.
 */
class TableBlockHandler : public BlockHandler {
public:
    bool canHandle(MD_BLOCKTYPE blockType) const override;
    
    int enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;
    int leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const override;

private:
    void calculateColumnWidths(TableState& table) const;
    void renderTable(RenderState& state) const;
    void addBlockquotePrefixes(RenderState& state) const;
    void addIndentation(RenderState& state) const;
};

} // namespace markdown
