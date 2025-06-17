#include "markdown/common/text_utils.h"
#include "markdown/common/ansi_colors.h"
#include <stdexcept>

namespace markdown {

/**
 * Decodes JSON escape sequences and Unicode HTML entities in the input text.
 * This handles strings that come from JSON-encoded markdown content.
 */
std::string TextUtils::decodeJsonAndUnicodeEscapes(const std::string& text) {
    std::string result;
    result.reserve(text.length());
    
    for (size_t i = 0; i < text.length(); ++i) {
        if (text[i] == '\\' && i + 1 < text.length()) {
            char next = text[i + 1];
            switch (next) {
                case 'n':
                    result += '\n';
                    ++i; // Skip the next character
                    break;
                case 't':
                    result += '\t';
                    ++i;
                    break;
                case 'r':
                    result += '\r';
                    ++i;
                    break;
                case '\\':
                    result += '\\';
                    ++i;
                    break;
                case '"':
                    result += '"';
                    ++i;
                    break;
                case '/':
                    result += '/';
                    ++i;
                    break;
                case 'u':
                    // Handle Unicode escape sequences like \u003c, \u003e
                    if (i + 5 < text.length()) {
                        std::string hexCode = text.substr(i + 2, 4);
                        try {
                            // Convert hex string to integer
                            unsigned int codePoint = std::stoul(hexCode, nullptr, 16);
                            result += unicodeToUtf8(codePoint);
                            i += 5; // Skip \uXXXX
                        } catch (const std::exception&) {
                            // If parsing fails, keep the original sequence
                            result += text[i];
                        }
                    } else {
                        // Not enough characters for a valid \uXXXX sequence
                        result += text[i];
                    }
                    break;
                default:
                    // Unknown escape sequence, keep both characters
                    result += text[i];
                    result += next;
                    ++i;
                    break;
            }
        } else {
            result += text[i];
        }
    }
    
    return result;
}

/**
 * Gets the display width of text (handles basic UTF-8)
 */
size_t TextUtils::getDisplayWidth(const std::string& text) {
    // Simple implementation - count characters, not bytes
    // This is a basic version; a full implementation would handle Unicode properly
    size_t width = 0;
    for (size_t i = 0; i < text.length(); ) {
        unsigned char c = text[i];
        if (c < 0x80) {
            // ASCII character
            width++;
            i++;
        } else if ((c & 0xE0) == 0xC0) {
            // 2-byte UTF-8
            width++;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            // 3-byte UTF-8
            width++;
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            // 4-byte UTF-8
            width++;
            i += 4;
        } else {
            // Invalid UTF-8, skip
            i++;
        }
    }
    return width;
}

/**
 * Pads cell content to specified width
 */
std::string TextUtils::padCell(const std::string& content, size_t width, bool isHeader) {
    size_t contentWidth = getDisplayWidth(content);
    size_t padding = (width > contentWidth) ? width - contentWidth : 0;
    
    std::string result = " " + content;
    for (size_t i = 0; i < padding - 1; i++) {
        result += " ";
    }
    
    if (isHeader) {
        return AnsiColors::BOLD + AnsiColors::WHITE + result + AnsiColors::RESET;
    } else {
        return result;
    }
}

/**
 * Converts a Unicode code point to UTF-8
 */
std::string TextUtils::unicodeToUtf8(unsigned int codePoint) {
    std::string result;
    
    // Convert common HTML entities to their ASCII equivalents
    switch (codePoint) {
        case 0x003c: // \u003c -> <
            result += '<';
            break;
        case 0x003e: // \u003e -> >
            result += '>';
            break;
        case 0x0026: // \u0026 -> &
            result += '&';
            break;
        case 0x0022: // \u0022 -> "
            result += '"';
            break;
        case 0x0027: // \u0027 -> '
            result += '\'';
            break;
        default:
            // For other Unicode characters, convert to UTF-8
            if (codePoint <= 0x7F) {
                // ASCII range
                result += static_cast<char>(codePoint);
            } else if (codePoint <= 0x7FF) {
                // 2-byte UTF-8
                result += static_cast<char>(0xC0 | (codePoint >> 6));
                result += static_cast<char>(0x80 | (codePoint & 0x3F));
            } else {
                // 3-byte UTF-8 (sufficient for \uXXXX range)
                result += static_cast<char>(0xE0 | (codePoint >> 12));
                result += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (codePoint & 0x3F));
            }
            break;
    }
    
    return result;
}

} // namespace markdown
