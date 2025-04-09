#include "markdown.h"
#include <iostream>
#include <fstream>
#include <cstdio>

// Documentation added for functions and classes in this file.
/**
 * Renders a Markdown string to the terminal using mdcat.
 * @param markdown The Markdown string to render.
 */
void renderMarkdown(const std::string &markdown) {
    std::string tempFile = "/tmp/markdown.md";
    std::ofstream outFile(tempFile);
    if (!outFile) {
        std::cerr << "Failed to create temporary file for Markdown rendering." << std::endl;
        return;
    }
    outFile << markdown;
    outFile.close();

    std::string command = "mdcat " + tempFile;
    int ret_code = std::system(command.c_str());
    if (ret_code != 0) {
        std::cerr << "Failed to render Markdown with mdcat." << std::endl;
    }

    std::remove(tempFile.c_str());
}