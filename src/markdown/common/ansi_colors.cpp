#include "markdown/common/ansi_colors.h"

namespace markdown {

// Text formatting
const std::string AnsiColors::RESET = "\033[0m";
const std::string AnsiColors::BOLD = "\033[1m";
const std::string AnsiColors::DIM = "\033[2m";
const std::string AnsiColors::UNDERLINE = "\033[4m";

// Foreground colors
const std::string AnsiColors::BLACK = "\033[30m";
const std::string AnsiColors::RED = "\033[31m";
const std::string AnsiColors::GREEN = "\033[32m";
const std::string AnsiColors::YELLOW = "\033[33m";
const std::string AnsiColors::BLUE = "\033[34m";
const std::string AnsiColors::MAGENTA = "\033[35m";
const std::string AnsiColors::CYAN = "\033[36m";
const std::string AnsiColors::WHITE = "\033[37m";

// Background colors
const std::string AnsiColors::BG_BLACK = "\033[40m";
const std::string AnsiColors::BG_GRAY = "\033[100m";

/**
 * Returns appropriate color for header level with hierarchical intensity
 * H1 (brightest) -> H6 (dimmest) for clear visual hierarchy
 */
std::string AnsiColors::getHeaderColor(int level) {
    switch (level) {
        case 1: return WHITE;             // H1: Bright white + bold (from caller)
        case 2: return WHITE;             // H2: White + bold (from caller)
        case 3: return YELLOW;            // H3: Yellow + bold (from caller)
        case 4: return GREEN;             // H4: Green + bold (from caller)
        case 5: return CYAN;              // H5: Cyan + bold (from caller)
        case 6: return DIM + WHITE;       // H6: Dimmed white + bold (from caller)
        default: return WHITE;
    }
}

/**
 * Escapes any existing ANSI sequences in text to prevent conflicts
 */
std::string AnsiColors::escapeAnsiSequences(const std::string& text) {
    // For now, just return the text as-is
    // In a more robust implementation, we might escape existing ANSI codes
    return text;
}

} // namespace markdown
