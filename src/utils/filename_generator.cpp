#include "filename_generator.h"
#include "file_operations.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <regex>

// Common English stop words to filter out when extracting keywords
const std::set<std::string> FilenameGenerator::STOP_WORDS = {
    "a", "an", "and", "are", "as", "at", "be", "by", "for", "from",
    "has", "he", "in", "is", "it", "its", "of", "on", "that", "the",
    "to", "was", "will", "with", "would", "you", "your", "i", "me",
    "my", "we", "our", "us", "this", "these", "those", "can", "could",
    "should", "do", "does", "did", "have", "had", "but", "or", "not",
    "if", "when", "where", "why", "how", "what", "who", "which", "am"
};

std::string FilenameGenerator::generateFromPrompt(const std::string& prompt, size_t maxLength) {
    if (prompt.empty()) {
        return "untitled";
    }

    // Extract meaningful keywords from the prompt
    std::vector<std::string> keywords = extractKeywords(prompt, 5);
    
    // Join keywords with underscores
    std::string filename;
    for (size_t i = 0; i < keywords.size(); ++i) {
        if (i > 0) {
            filename += "_";
        }
        filename += keywords[i];
        
        // Stop if we're approaching the length limit
        if (filename.length() >= maxLength) {
            break;
        }
    }
    
    // If no meaningful keywords were extracted, use first few words
    if (filename.empty()) {
        std::vector<std::string> words = splitIntoWords(prompt);
        for (size_t i = 0; i < std::min(words.size(), size_t(3)); ++i) {
            if (i > 0) {
                filename += "_";
            }
            filename += sanitizeFilename(words[i]);
            
            if (filename.length() >= maxLength) {
                break;
            }
        }
    }
    
    // Ensure we have something meaningful
    if (filename.empty()) {
        filename = "chat";
    }
    
    // Truncate if still too long
    filename = truncateAtWordBoundary(filename, maxLength);
    
    // Final sanitization
    filename = sanitizeFilename(filename);
    
    // Ensure it doesn't start or end with underscores/hyphens
    while (!filename.empty() && (filename.front() == '_' || filename.front() == '-')) {
        filename.erase(0, 1);
    }
    while (!filename.empty() && (filename.back() == '_' || filename.back() == '-')) {
        filename.pop_back();
    }
    
    // Final fallback
    if (filename.empty()) {
        filename = "chat";
    }
    
    return filename;
}

std::string FilenameGenerator::ensureUniqueFilename(const std::string& directory, 
                                                  const std::string& baseFilename, 
                                                  const std::string& extension) {
    std::string candidateFilename = baseFilename + extension;
    std::string fullPath = directory + "/" + candidateFilename;
    
    // If the file doesn't exist, we can use the base filename
    if (!FileOperations::exists(fullPath)) {
        return baseFilename;
    }
    
    // Try appending numbers until we find a unique filename
    int counter = 1;
    do {
        candidateFilename = baseFilename + "_" + std::to_string(counter) + extension;
        fullPath = directory + "/" + candidateFilename;
        counter++;
    } while (FileOperations::exists(fullPath) && counter < 1000); // Reasonable limit
    
    // Return just the base part (without extension)
    return baseFilename + "_" + std::to_string(counter - 1);
}

std::vector<std::string> FilenameGenerator::extractKeywords(const std::string& text, size_t maxWords) {
    std::vector<std::string> words = splitIntoWords(text);
    std::vector<std::string> keywords;
    
    for (const std::string& word : words) {
        if (keywords.size() >= maxWords) {
            break;
        }
        
        std::string lowerWord = toLower(word);
        
        // Skip stop words, very short words, and words that are mostly numbers
        if (isStopWord(lowerWord) || word.length() < 2) {
            continue;
        }
        
        // Skip words that are mostly numbers or special characters
        bool hasLetter = false;
        for (char c : word) {
            if (std::isalpha(c)) {
                hasLetter = true;
                break;
            }
        }
        
        if (!hasLetter) {
            continue;
        }
        
        // Prioritize longer, more meaningful words
        std::string sanitized = sanitizeFilename(word);
        if (!sanitized.empty() && sanitized.length() >= 2) {
            keywords.push_back(sanitized);
        }
    }
    
    return keywords;
}

std::string FilenameGenerator::sanitizeFilename(const std::string& input) {
    std::string result;
    result.reserve(input.length());
    
    for (char c : input) {
        if (std::isalnum(c)) {
            result += std::tolower(c);
        } else if (c == ' ' || c == '-' || c == '_') {
            // Convert spaces and hyphens to underscores, but avoid consecutive underscores
            if (!result.empty() && result.back() != '_') {
                result += '_';
            }
        }
        // Skip all other characters (punctuation, special chars, etc.)
    }
    
    return result;
}

std::string FilenameGenerator::truncateAtWordBoundary(const std::string& text, size_t maxLength) {
    if (text.length() <= maxLength) {
        return text;
    }
    
    // Find the last underscore before maxLength
    size_t lastUnderscore = text.rfind('_', maxLength);
    
    if (lastUnderscore != std::string::npos && lastUnderscore > maxLength / 2) {
        // Truncate at the word boundary if it's not too short
        return text.substr(0, lastUnderscore);
    }
    
    // Otherwise, just truncate at the character limit
    return text.substr(0, maxLength);
}

std::string FilenameGenerator::toLower(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool FilenameGenerator::isStopWord(const std::string& word) {
    return STOP_WORDS.find(word) != STOP_WORDS.end();
}

std::vector<std::string> FilenameGenerator::splitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::istringstream iss(text);
    std::string word;
    
    // Split on whitespace and common punctuation
    std::regex wordRegex("[a-zA-Z0-9_-]+");
    std::sregex_iterator begin(text.begin(), text.end(), wordRegex);
    std::sregex_iterator end;
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        std::string matchStr = match.str();
        if (!matchStr.empty()) {
            words.push_back(matchStr);
        }
    }
    
    return words;
}
