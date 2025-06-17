#ifndef MARKDOWN_ANSI_COLORS_H
#define MARKDOWN_ANSI_COLORS_H

#include <string>

namespace markdown {

/**
 * @brief ANSI color codes and utilities for terminal output formatting.
 * 
 * This class provides ANSI escape sequences for terminal colors and text formatting,
 * along with utility functions for color management in markdown rendering.
 * 
 * Color Philosophy:
 * - Headers: Use hierarchical white/yellow/green/cyan for clear visual hierarchy
 * - Code elements: Readable without jarring backgrounds
 * - Lists: Subtle dim white markers
 * - Structural elements: Consistent dim white styling
 * - Terminal-friendly colors that work well in both light/dark themes
 */
class AnsiColors {
public:
    // Text formatting
    static const std::string RESET;
    static const std::string BOLD;
    static const std::string DIM;
    static const std::string UNDERLINE;
    
    // Foreground colors
    static const std::string BLACK;
    static const std::string RED;
    static const std::string GREEN;
    static const std::string YELLOW;
    static const std::string BLUE;
    static const std::string MAGENTA;
    static const std::string CYAN;
    static const std::string WHITE;
    
    // Background colors
    static const std::string BG_BLACK;
    static const std::string BG_GRAY;
    
    /**
     * @brief Gets appropriate color for markdown header level.
     * 
     * Returns hierarchical colors for headers H1-H6, with H1 being brightest
     * and H6 being dimmest for clear visual hierarchy.
     * 
     * @param level Header level (1-6)
     * @return ANSI color code string for the header level
     */
    static std::string getHeaderColor(int level);
    
    /**
     * @brief Escapes existing ANSI sequences in text to prevent conflicts.
     * 
     * This function prevents text content from interfering with markdown
     * formatting by escaping any existing ANSI sequences.
     * 
     * @param text The text to escape
     * @return Text with ANSI sequences escaped
     */
    static std::string escapeAnsiSequences(const std::string& text);
};

} // namespace markdown

#endif // MARKDOWN_ANSI_COLORS_H
