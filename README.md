# AI Chat Application
This project is a lightweight C++ application for interacting with AI models in the terminal. It allows users to chat with AI, save conversation histories, and view responses in Markdown format for better clarity.

## Features
- **Multi-Provider Support**: Easily switch between different AI providers (Groq, OpenRouter, etc.).
- **Chat with AI Models**: Communicate with AI models using a simple command-line interface.
- **History Management**: Save and manage chat histories for future reference.
- **Markdown Rendering**: Render AI responses in Markdown format for better readability.

## Prerequisites
Before building and running the application, ensure you have the following installed on your system:
- A C++ compiler (e.g., GCC or Clang)
- CMake (version 3.10 or higher)
- JsonCpp library
- `curl` command-line tool
- `mdcat` for rendering Markdown in the terminal

## Building the Project
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd ai
   ```

2. Create a build directory and navigate to it:
   ```bash
   mkdir build && cd build
   ```

3. Run CMake to configure the project:
   ```bash
   cmake ..
   ```

4. Build the project:
   ```bash
   make
   ```

## Configuration
The application supports multiple AI providers through a flexible configuration system:

### Provider Configuration Files
Create provider-specific configuration files in `~/.config/ai/` directory:

```
~/.config/ai/groq.conf
~/.config/ai/openrouter.conf
~/.config/ai/anthropic.conf
# etc...
```

Each provider file should contain:
```
API_URL=https://api.provider.com/path
API_KEY=your-api-key-here
DEFAULT_MODEL=provider-model-name
```

### Default Provider
Set your default provider in the main config file:
```
# ~/.config/ai/config
DEFAULT_PROVIDER=groq
```

### Environment Variables
You can also use environment variables for configuration:
```bash
# Set provider-specific variables
export GROQ_API_KEY="your-api-key"
export OPENROUTER_API_URL="https://openrouter.ai/api/v1"

# Or use generic variables
export API_KEY="your-api-key"
export API_URL="https://api.provider.com/v1"
export DEFAULT_MODEL="model-name"
```

## Running the Application
1. Navigate to the build directory:
   ```bash
   cd build
   ```

2. Run the application with default provider:
   ```bash
   ./ai "Your prompt here"
   ```

3. Run with a specific provider:
   ```bash
   ./ai --provider=openrouter "Your prompt here"
   # OR
   ./ai -p groq "Your prompt here"
   ```

4. Start a new chat:
   ```bash
   ./ai new "Your initial prompt"
   ```

5. List available models:
   ```bash
   ./ai list
   # OR
   ./ai --provider=anthropic list
   ```

6. View your chat history:
   ```bash
   ./ai history
   ```

## Documentation
Detailed documentation for the project is available in the `docs/html` directory. Open the `index.html` file in a web browser to view the documentation:
```bash
xdg-open docs/html/index.html
```

## Contributing
Contributions are welcome! If you have suggestions for improvements or new features, feel free to open an issue or submit a pull request.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

## Acknowledgments
- [JsonCpp](https://github.com/open-source-parsers/jsoncpp) for JSON parsing.
- [mdcat](https://github.com/lunaryorn/mdcat) for Markdown rendering.
- [curl](https://curl.se/) for HTTP requests.