#include "markdown/common/word_wrapper.h"
#include <iostream>
#include <sstream>
#include <sys/ioctl.h>
#include <unistd.h>

namespace markdown {

WordWrapper::WordWrapper() : enabled_(true), customWidth_(0) {
}

WordWrapper::WordWrapper(size_t terminalWidth) : enabled_(true), customWidth_(terminalWidth) {
}

size_t WordWrapper::detectTerminalWidth() const {
    if (customWidth_ > 0) {
        return customWidth_;
    }
    
    struct winsize terminalSize;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminalSize) == 0) {
        return terminalSize.ws_col;
    }
    return 80; // Default fallback width
}

size_t WordWrapper::calculateVisualWidth(const std::string& text) const {
    size_t visualWidth = 0;
    bool inAnsiSequence = false;
    
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\033' && i + 1 < text.size() && text[i + 1] == '[') {
            inAnsiSequence = true;
            ++i; // Skip the '['
        } else if (inAnsiSequence && text[i] == 'm') {
            inAnsiSequence = false;
        } else if (!inAnsiSequence) {
            visualWidth++;
        }
    }
    
    return visualWidth;
}

size_t WordWrapper::findLastWordBoundary(const std::string& text, size_t maxVisualWidth) const {
    size_t visualWidth = 0;
    size_t lastSpacePos = std::string::npos;
    bool inAnsiSequence = false;
    
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\033' && i + 1 < text.size() && text[i + 1] == '[') {
            inAnsiSequence = true;
            ++i; // Skip the '['
            continue;
        } else if (inAnsiSequence && text[i] == 'm') {
            inAnsiSequence = false;
            continue;
        } else if (!inAnsiSequence) {
            if (visualWidth >= maxVisualWidth) {
                break;
            }
            if (text[i] == ' ') {
                lastSpacePos = i;
            }
            visualWidth++;
        }
    }
    
    return lastSpacePos;
}

bool WordWrapper::isSpecialFormatting(const std::string& content) const {
    if (content.empty()) {
        return false;
    }
    
    // Don't wrap lines that start with special characters (using string comparison for Unicode)
    if (content.substr(0, 1) == "│" || content.substr(0, 1) == "┌" || content.substr(0, 1) == "├" || 
        content.substr(0, 1) == "└" || content.substr(0, 1) == "─" || content.substr(0, 1) == "┬" || 
        content.substr(0, 1) == "┼" || content.substr(0, 1) == "┴" || content.substr(0, 1) == "┐" || 
        content.substr(0, 1) == "┤" || content.substr(0, 1) == "┘" || content.substr(0, 1) == "•" ||
        (content.size() >= 4 && content.substr(0, 4) == "    ")) { // Indented code
        return true;
    }
    
    return false;
}

size_t WordWrapper::calculateLineIndentation(const std::string& line) const {
    size_t indentCount = 0;
    for (char c : line) {
        if (c == ' ' || c == '\t') {
            indentCount++;
        } else {
            break;
        }
    }
    return indentCount;
}

std::string WordWrapper::wrapText(const std::string& formattedText) const {
    if (!enabled_) {
        return formattedText;
    }
    
    size_t terminalWidth = detectTerminalWidth();
    std::vector<std::string> lines;
    std::istringstream stream(formattedText);
    std::string line;
    
    // Process each line separately
    while (std::getline(stream, line)) {
        if (line.empty()) {
            lines.push_back(line);
            continue;
        }
        
        // Calculate indentation of the current line
        size_t indentCount = calculateLineIndentation(line);
        std::string indent = line.substr(0, indentCount);
        std::string content = line.substr(indentCount);
        
        // Check if this line has special formatting that shouldn't be wrapped
        if (isSpecialFormatting(content)) {
            lines.push_back(line);
            continue;
        }
        
        // Calculate available width for content
        size_t availableWidth = terminalWidth > indentCount ? terminalWidth - indentCount : terminalWidth;
        
        // Wrap the content if it's too long
        size_t visualWidth = calculateVisualWidth(content);
        if (visualWidth <= availableWidth) {
            lines.push_back(line);
        } else {
            // Need to wrap this line
            std::string remaining = content;
            
            while (!remaining.empty() && calculateVisualWidth(remaining) > availableWidth) {
                size_t breakPoint = findLastWordBoundary(remaining, availableWidth);
                
                if (breakPoint == std::string::npos) {
                    // No good break point found, force break at available width
                    // This is a fallback for very long words
                    size_t visualPos = 0;
                    size_t bytePos = 0;
                    bool inAnsiSeq = false;
                    
                    for (size_t i = 0; i < remaining.size() && visualPos < availableWidth; ++i) {
                        if (remaining[i] == '\033' && i + 1 < remaining.size() && remaining[i + 1] == '[') {
                            inAnsiSeq = true;
                            ++i; // Skip the '['
                        } else if (inAnsiSeq && remaining[i] == 'm') {
                            inAnsiSeq = false;
                        } else if (!inAnsiSeq) {
                            visualPos++;
                        }
                        bytePos = i + 1;
                    }
                    
                    breakPoint = bytePos;
                }
                
                lines.push_back(indent + remaining.substr(0, breakPoint));
                remaining = remaining.substr(breakPoint);
                
                // Skip leading spaces in the remaining text
                while (!remaining.empty() && remaining[0] == ' ') {
                    remaining = remaining.substr(1);
                }
            }
            
            // Add the remaining part if any
            if (!remaining.empty()) {
                lines.push_back(indent + remaining);
            }
        }
    }
    
    // Join all lines back together
    std::string result;
    for (size_t i = 0; i < lines.size(); ++i) {
        result += lines[i];
        if (i < lines.size() - 1) {
            result += "\n";
        }
    }
    
    return result;
}

} // namespace markdown
