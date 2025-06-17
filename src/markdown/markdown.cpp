#include "markdown/markdown.h"
#include "markdown/terminal_markdown_renderer.h"
#include <iostream>

// Documentation added for functions and classes in this file.
/**
 * Renders a Markdown string to the terminal using native C++ markdown rendering.
 * This replaces the external mdcat dependency with a lightweight C++ solution.
 * @param markdown The Markdown string to render.
 */
void renderMarkdown(const std::string &markdown) {
    TerminalMarkdownRenderer renderer;
    std::string rendered = renderer.render(markdown);
    std::cout << rendered << std::flush;
}