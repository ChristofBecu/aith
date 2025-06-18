#ifndef FILENAME_GENERATOR_H
#define FILENAME_GENERATOR_H

#include <string>
#include <vector>
#include <set>

/**
 * Utility class for generating safe, concise, and meaningful filenames from user prompts.
 * Handles length constraints, character sanitization, keyword extraction, and collision avoidance.
 */
class FilenameGenerator {
public:
    /**
     * Generates a safe filename from a user prompt.
     * @param prompt The user prompt to generate a filename from.
     * @param maxLength Maximum length for the descriptive part (default: 50).
     * @return A sanitized filename suitable for filesystem use.
     */
    static std::string generateFromPrompt(const std::string& prompt, size_t maxLength = 50);

    /**
     * Ensures a filename is unique by appending a number if necessary.
     * @param directory The directory where the file will be created.
     * @param baseFilename The base filename (without extension).
     * @param extension The file extension (including dot, e.g., ".json").
     * @return A unique filename that doesn't exist in the directory.
     */
    static std::string ensureUniqueFilename(const std::string& directory, 
                                          const std::string& baseFilename, 
                                          const std::string& extension);

private:
    /**
     * Extracts meaningful keywords from text, filtering out stop words.
     * @param text The input text to extract keywords from.
     * @param maxWords Maximum number of words to extract.
     * @return Vector of extracted keywords in order of importance.
     */
    static std::vector<std::string> extractKeywords(const std::string& text, size_t maxWords = 5);

    /**
     * Sanitizes a string to be filesystem-safe.
     * @param input The input string to sanitize.
     * @return A sanitized string safe for use as a filename.
     */
    static std::string sanitizeFilename(const std::string& input);

    /**
     * Truncates text intelligently at word boundaries.
     * @param text The text to truncate.
     * @param maxLength Maximum length allowed.
     * @return Truncated text that doesn't break words when possible.
     */
    static std::string truncateAtWordBoundary(const std::string& text, size_t maxLength);

    /**
     * Converts text to lowercase for processing.
     * @param text The input text.
     * @return Lowercase version of the text.
     */
    static std::string toLower(const std::string& text);

    /**
     * Checks if a word is a common stop word that should be filtered out.
     * @param word The word to check.
     * @return True if the word is a stop word.
     */
    static bool isStopWord(const std::string& word);

    /**
     * Splits text into words, handling various delimiters.
     * @param text The text to split.
     * @return Vector of words.
     */
    static std::vector<std::string> splitIntoWords(const std::string& text);

    // Common English stop words to filter out
    static const std::set<std::string> STOP_WORDS;
};

#endif // FILENAME_GENERATOR_H
