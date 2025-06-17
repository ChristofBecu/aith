#pragma once

#include "markdown/blocks/block_handler.h"
#include <memory>
#include <vector>
#include <md4c.h>

namespace markdown {

/**
 * @brief Factory for creating and managing block handlers
 * 
 * Provides a centralized way to handle different markdown block types
 * through the appropriate handler classes.
 */
class BlockHandlerFactory {
public:
    BlockHandlerFactory();
    ~BlockHandlerFactory() = default;
    
    // Non-copyable but movable
    BlockHandlerFactory(const BlockHandlerFactory&) = delete;
    BlockHandlerFactory& operator=(const BlockHandlerFactory&) = delete;
    BlockHandlerFactory(BlockHandlerFactory&&) = default;
    BlockHandlerFactory& operator=(BlockHandlerFactory&&) = default;
    
    /**
     * Handles entering a block by delegating to the appropriate handler
     */
    int handleEnterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const;
    
    /**
     * Handles leaving a block by delegating to the appropriate handler
     */
    int handleLeaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const;

private:
    std::vector<std::unique_ptr<BlockHandler>> handlers;
    
    void initializeHandlers();
    const BlockHandler* findHandler(MD_BLOCKTYPE blockType) const;
};

} // namespace markdown
