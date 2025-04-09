# AI Chat Application
This project is a lightweight C++ application for interacting with AI models in the terminal. It allows users to chat with AI, save conversation histories, and view responses in Markdown format for better clarity.

## Features
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
The application uses the following environment variables for configuration:
- `GROQ_API_URL`: The base URL for the AI API (default: `https://api.groq.com/openai/v1`).
- `DEFAULT_MODEL`: The default AI model to use (default: `llama-3.3-70b-specdec`).

You can set these variables in your shell before running the application:
```bash
export GROQ_API_URL="https://your-api-url.com"
export DEFAULT_MODEL="your-model-name"
```

## Running the Application
1. Navigate to the build directory:
   ```bash
   cd build
   ```

2. Run the application:
   ```bash
   ./ai
   ```

3. Follow the on-screen instructions to interact with the AI model.

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