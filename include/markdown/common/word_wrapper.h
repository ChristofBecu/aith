#pragma once

#include <string>
#include <vector>

namespace markdown {

/**
 * @brief Handles intelligent word wrapping for terminal output with ANSI support
 * 
 * This class provides post-processing word wrapping that correctly handles:
 * - ANSI escape sequences (colors, formatting)
 * - Word boundary preservation
 * - Special formatting preservation (code blocks, tables)
 * - Indentation awareness
 * - Terminal width detection
 */
class WordWrapper {
public:
    /**
     * @brief Default constructor with auto-detected terminal width
     */
    WordWrapper();
    
    /**
     * @brief Constructor with custom terminal width
     * @param terminalWidth Custom width in characters
     */
    explicit WordWrapper(size_t terminalWidth);
    
    /**
     * @brief Enable or disable word wrapping
     * @param enabled Whether to enable word wrapping
     */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    /**
     * @brief Set custom terminal width
     * @param width Terminal width in characters (0 for auto-detection)
     */
    void setTerminalWidth(size_t width) { customWidth_ = width; }
    
    /**
     * @brief Check if word wrapping is enabled
     * @return True if word wrapping is enabled
     */
    bool isEnabled() const { return enabled_; }
    
    /**
     * @brief Get the current terminal width setting
     * @return Terminal width (0 means auto-detection)
     */
    size_t getTerminalWidth() const { return customWidth_; }
    
    /**
     * @brief Apply word wrapping to formatted text
     * @param formattedText Text with ANSI formatting already applied
     * @return Word-wrapped text preserving formatting
     */
    std::string wrapText(const std::string& formattedText) const;

private:
    bool enabled_;
    size_t customWidth_;
    
    /**
     * @brief Detect terminal width using system calls
     * @return Terminal width in characters
     */
    size_t detectTerminalWidth() const;
    
    /**
     * @brief Calculate visual width ignoring ANSI escape sequences
     * @param text Text to measure
     * @return Visual width in characters
     */
    size_t calculateVisualWidth(const std::string& text) const;
    
    /**
     * @brief Find last word boundary before maximum visual width
     * @param text Text to search
     * @param maxVisualWidth Maximum visual width
     * @return Position of last word boundary or string::npos
     */
    size_t findLastWordBoundary(const std::string& text, size_t maxVisualWidth) const;
    
    /**
     * @brief Check if a line contains special formatting that shouldn't be wrapped
     * @param content Line content to check
     * @return True if line contains special formatting
     */
    bool isSpecialFormatting(const std::string& content) const;
    
    /**
     * @brief Calculate indentation of a line
     * @param line Line to analyze
     * @return Number of leading whitespace characters
     */
    size_t calculateLineIndentation(const std::string& line) const;
};

} // namespace markdown
