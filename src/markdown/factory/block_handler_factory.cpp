#include "markdown/factory/block_handler_factory.h"
#include "markdown/blocks/header_block_handler.h"
#include "markdown/blocks/paragraph_block_handler.h"
#include "markdown/blocks/code_block_handler.h"
#include "markdown/blocks/list_block_handler.h"
#include "markdown/blocks/blockquote_block_handler.h"
#include "markdown/blocks/horizontal_rule_block_handler.h"
#include "markdown/blocks/table_block_handler.h"

namespace markdown {

BlockHandlerFactory::BlockHandlerFactory() {
    initializeHandlers();
}

void BlockHandlerFactory::initializeHandlers() {
    handlers.emplace_back(std::make_unique<HeaderBlockHandler>());
    handlers.emplace_back(std::make_unique<ParagraphBlockHandler>());
    handlers.emplace_back(std::make_unique<CodeBlockHandler>());
    handlers.emplace_back(std::make_unique<ListBlockHandler>());
    handlers.emplace_back(std::make_unique<BlockquoteBlockHandler>());
    handlers.emplace_back(std::make_unique<HorizontalRuleBlockHandler>());
    handlers.emplace_back(std::make_unique<TableBlockHandler>());
}

int BlockHandlerFactory::handleEnterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    const BlockHandler* handler = findHandler(blockType);
    if (handler) {
        return handler->enterBlock(blockType, detail, state);
    }
    return 0;
}

int BlockHandlerFactory::handleLeaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    const BlockHandler* handler = findHandler(blockType);
    int result = 0;
    if (handler) {
        result = handler->leaveBlock(blockType, detail, state);
    }
    
    // Centralized trailing spacing logic
    addTrailingSpacing(blockType, state);
    
    return result;
}

const BlockHandler* BlockHandlerFactory::findHandler(MD_BLOCKTYPE blockType) const {
    for (const auto& handler : handlers) {
        if (handler->canHandle(blockType)) {
            return handler.get();
        }
    }
    return nullptr;
}

void BlockHandlerFactory::addTrailingSpacing(MD_BLOCKTYPE blockType, RenderState& state) const {
    switch (blockType) {
        case MD_BLOCK_H:
            // Headers get double newlines for clear separation
            state.output += "\n\n";
            break;
            
        case MD_BLOCK_P:
            // Paragraphs get double newlines for clear separation
            state.output += "\n\n";
            break;
            
        case MD_BLOCK_CODE:
            // Code blocks get double newlines for clear separation
            state.output += "\n\n";
            break;
            
        case MD_BLOCK_UL:
        case MD_BLOCK_OL:
            // List spacing depends on nesting level
            if (state.listLevel == 0) {
                // End of outermost list gets double newlines
                state.output += "\n\n";
            } else {
                // Nested list end gets single newline
                state.output += "\n";
            }
            break;
            
        case MD_BLOCK_QUOTE:
            // Blockquotes get single newline
            state.output += "\n";
            break;
            
        case MD_BLOCK_HR:
            // Horizontal rules get double newlines for clear separation
            state.output += "\n\n";
            break;
            
        case MD_BLOCK_TABLE:
            // Tables get double newlines for clear separation
            state.output += "\n\n";
            break;
            
        case MD_BLOCK_LI:
        case MD_BLOCK_THEAD:
        case MD_BLOCK_TBODY:
        case MD_BLOCK_TR:
        case MD_BLOCK_TD:
        case MD_BLOCK_TH:
            // These sub-blocks don't need additional spacing
            break;
    }
}

} // namespace markdown
