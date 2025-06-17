#include "markdown/blocks/code_block_handler.h"
#include "markdown/common/ansi_colors.h"
#include <md4c.h>
#include <string>

namespace markdown {

bool CodeBlockHandler::canHandle(MD_BLOCKTYPE blockType) const {
    return blockType == MD_BLOCK_CODE;
}

int CodeBlockHandler::enterBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    auto* codeDetail = static_cast<MD_BLOCK_CODE_DETAIL*>(detail);
    state.inCodeBlock = true;
    state.output += "\n";
    addIndentation(state);
    
    if (codeDetail->lang.text != nullptr) {
        std::string lang(codeDetail->lang.text, codeDetail->lang.size);
        state.output += AnsiColors::GREEN + "(" + lang + ")" + AnsiColors::RESET + "\n";
        addIndentation(state);
    }
    
    // Set color for the code content that will follow
    state.output += AnsiColors::DIM + AnsiColors::CYAN;
    
    return 0;
}

int CodeBlockHandler::leaveBlock(MD_BLOCKTYPE blockType, void* detail, RenderState& state) const {
    if (!canHandle(blockType)) return 0;
    
    state.inCodeBlock = false;
    state.output += AnsiColors::RESET + "\n\n";
    
    return 0;
}

void CodeBlockHandler::addIndentation(RenderState& state) const {
    for (int i = 0; i < state.indentLevel; i++) {
        state.output += "  ";
    }
}

} // namespace markdown
