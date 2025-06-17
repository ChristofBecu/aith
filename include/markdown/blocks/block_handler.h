#pragma once

#include "markdown/common/render_state.h"
#include <md4c.h>

namespace markdown {

/**
 * @brief Abstract base class for handling markdown block elements
 * 
 * This class defines the interface for handling different types of markdown
 * blocks during parsing. Each block type (headers, paragraphs, code blocks,
 * lists, etc.) should have its own concrete handler implementation.
 */
class BlockHandler {
public:
    virtual ~BlockHandler() = default;
    
    /**
     * @brief Checks if this handler can process the given block type
     * @param blockType The MD4C block type to check
     * @return true if this handler can process the block type, false otherwise
     */
    virtual bool canHandle(MD_BLOCKTYPE blockType) const = 0;
    
    /**
     * @brief Handles entering a block element
     * @param blockType The type of block being entered
     * @param detail Block-specific details from MD4C
     * @param state Current rendering state
     * @return 0 on success, non-zero on error
     */
    virtual int enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const = 0;
    
    /**
     * @brief Handles leaving a block element
     * @param blockType The type of block being left
     * @param detail Block-specific details from MD4C
     * @param state Current rendering state
     * @return 0 on success, non-zero on error
     */
    virtual int leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const = 0;
};

} // namespace markdown
