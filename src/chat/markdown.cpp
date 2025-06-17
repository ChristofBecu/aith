#include "markdown.h"
#include "markdown_renderer.h"
#include <iostream>

// Documentation added for functions and classes in this file.
/**
 * Renders a Markdown string to the terminal using native C++ markdown rendering.
 * This replaces the external mdcat dependency with a lightweight C++ solution.
 * @param markdown The Markdown string to render.
 */
void renderMarkdown(const std::string &markdown) {
    std::cout << markdown << std::endl << std::flush;
    TerminalMarkdownRenderer renderer;
    std::string rendered = renderer.render(markdown);
    std::cout << rendered << std::flush;
}