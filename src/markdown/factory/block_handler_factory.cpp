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
    if (handler) {
        return handler->leaveBlock(blockType, detail, state);
    }
    return 0;
}

const BlockHandler* BlockHandlerFactory::findHandler(MD_BLOCKTYPE blockType) const {
    for (const auto& handler : handlers) {
        if (handler->canHandle(blockType)) {
            return handler.get();
        }
    }
    return nullptr;
}

} // namespace markdown
