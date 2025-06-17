#ifndef MARKDOWN_TEXT_UTILS_H
#define MARKDOWN_TEXT_UTILS_H

#include <string>

namespace markdown {

/**
 * @brief Text processing utilities for markdown rendering.
 * 
 * This class provides utilities for text processing, encoding/decoding,
 * and display width calculation for proper markdown formatting.
 */
class TextUtils {
public:
    /**
     * @brief Decodes JSON escape sequences and Unicode HTML entities.
     * 
     * This function handles strings that come from JSON-encoded markdown content,
     * converting escape sequences like \n, \t, \uXXXX back to their actual characters.
     * Also converts common Unicode HTML entities to their ASCII equivalents.
     * 
     * @param text The text containing escape sequences to decode
     * @return Decoded text with escape sequences converted
     */
    static std::string decodeJsonAndUnicodeEscapes(const std::string& text);
    
    /**
     * @brief Gets the display width of text handling basic UTF-8.
     * 
     * Calculates the visual width of text for proper alignment in tables and formatting.
     * Handles basic UTF-8 multibyte characters correctly.
     * 
     * @param text The text to measure
     * @return Display width in characters (not bytes)
     */
    static size_t getDisplayWidth(const std::string& text);
    
    /**
     * @brief Pads cell content to specified width with proper formatting.
     * 
     * Pads text content to the specified width for table alignment, with optional
     * header formatting (bold/white for header cells).
     * 
     * @param content The cell content to pad
     * @param width The target width for the cell
     * @param isHeader Whether this is a header cell (applies bold formatting)
     * @return Padded and formatted cell content
     */
    static std::string padCell(const std::string& content, size_t width, bool isHeader = false);

private:
    /**
     * @brief Converts a Unicode code point to UTF-8.
     * 
     * Helper function for Unicode escape sequence processing.
     * 
     * @param codePoint The Unicode code point to convert
     * @return UTF-8 encoded string
     */
    static std::string unicodeToUtf8(unsigned int codePoint);
};

} // namespace markdown

#endif // MARKDOWN_TEXT_UTILS_H
