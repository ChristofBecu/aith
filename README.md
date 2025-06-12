# AITH - And It Talks, Human

This project is a lightweight C++ application for interacting with aith models in the terminal. AITH features a modular architecture with dedicated classes for system operations, configuration management, provider handling, and model blacklisting.

## Features

- **Multi-Provider Support**: Easily switch between different aith providers (Groq, OpenRouter, etc.).
- **Chat with aith Models**: Communicate with aith models using a simple command-line interface.
- **History Management**: Save and manage chat histories for future reference.
- **Markdown Rendering**: Render aith responses in Markdown format for better readability.
- **Model Blacklisting**: Keep track of problematic models across different providers with automatic blacklisting.
- **Performance Benchmarking**: Test and compare model response times with built-in benchmarking tools.
- **Modular Architecture**: Clean separation of concerns with SystemUtils, ConfigManager, ProviderManager, and ModelBlacklist classes.

## Prerequisites

Before building and running the application, ensure you have the following installed on your system:

- A C++ compiler with C++17 support (e.g., GCC or Clang)
- CMake (version 3.10 or higher)
- CURL library (libcurl-dev on Ubuntu/Debian)
- JsonCpp library (libjsoncpp-dev on Ubuntu/Debian)
- `mdcat` for rendering Markdown in the terminal (optional but recommended)

## Building the Project

1. Clone the repository:

   ```bash
   git clone <repository-url>
   cd aith
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

The application uses a flexible configuration system with the following priority order:

1. **Environment Variables** (highest priority)
2. **Provider-specific configuration files**
3. **Main configuration file**
4. **Built-in defaults** (lowest priority)

### Configuration File Structure

Create provider-specific configuration files in `~/.config/ai/` directory:

```text
~/.config/ai/groq.conf
~/.config/ai/openrouter.conf
~/.config/ai/anthropic.conf
# etc...
```

Each provider file should contain:

```bash
API_URL=https://api.provider.com/path
API_KEY=your-api-key-here
DEFAULT_MODEL=provider-model-name
```

### Main Configuration

Set your default provider in the main config file:

```bash
# ~/.config/ai/config
DEFAULT_PROVIDER=groq
```

### Environment Variables

Environment variables take highest priority and follow this pattern:

```bash
# Provider-specific variables (recommended)
export GROQ_API_KEY="your-api-key"
export OPENROUTER_API_URL="https://openrouter.ai/api/v1"
export GROQ_DEFAULT_MODEL="llama3-70b-8192"

# Generic variables (fallback)
export API_KEY="your-api-key"
export API_URL="https://api.provider.com/v1"
export DEFAULT_MODEL="model-name"

# Provider selection
export AGENT="groq"  # Sets default provider
```

## Running the Application

Basic usage follows this pattern:

```bash
./aith [--provider=NAME | -p NAME] [list | history | test | blacklist | new "prompt" | "prompt"] [model (optional)]
```

### Common Commands

1. **Navigate to the build directory:**

   ```bash
   cd build
   ```

2. **Run with default provider:**

   ```bash
   ./aith "Your prompt here"
   ```

3. **Run with a specific provider:**

   ```bash
   ./aith --provider=openrouter "Your prompt here"
   # OR
   ./aith -p groq "Your prompt here"
   ```

4. **Start a new chat:**

   ```bash
   ./aith new "Your initial prompt"
   ```

5. **List available models:**

   ```bash
   ./aith list
   # OR with specific provider
   ./aith --provider=anthropic list
   ```

6. **View your chat history:**

   ```bash
   ./aith history
   ```

7. **Run performance benchmarks:**

   ```bash
   # Test all models with default prompt
   ./aith test
   
   # Test with custom prompt
   ./aith test "How are you?"
   
   # Test specific provider's models
   ./aith --provider=openrouter test "Explain AI"
   ```

8. **Manage model blacklist:**

   ```bash
   # List blacklisted models
   ./aith blacklist list
   
   # Add a model to the blacklist
   ./aith blacklist add <provider> <model_name> [reason]
   
   # Remove a model from the blacklist
   ./aith blacklist remove <provider> <model_name>
   ```

## Model Blacklist

The application maintains a sophisticated blacklist system to automatically and manually track problematic models. Models are blacklisted per provider, allowing fine-grained control over which models to avoid.

### Automatic Blacklisting

During benchmark testing (`./aith test`), models are automatically blacklisted if they:

- Return invalid API response formats
- Don't support chat completions
- Fail to respond properly to test prompts

Auto-blacklisted models include the reason and timestamp for tracking.

### Manual Blacklist Management

You can manually manage the blacklist using these commands:

```bash
# List all blacklisted models
./aith blacklist list

# Add a model to blacklist
./aith blacklist add groq llama-problematic-model "Too many hallucinations"

# Remove a model from blacklist  
./aith blacklist remove groq llama-problematic-model
```

### Blacklist File Format

The blacklist is stored at `~/.config/ai/blacklist` in a pipe-separated format:

```text
provider | model | reason # Added on date and time
```

Example entries:

```text
groq | whisper-large-v3 | Auto-blacklisted: Invalid API response format # Added on Thu Jan 02 10:15:23 2025
openrouter | claude-instant-1.2 | Very slow responses # Added on Thu Jan 02 11:30:45 2025
```

### Blacklist Features

- **Provider-Specific**: Models are blacklisted per provider, as the same model may perform differently across providers
- **Visual Indicators**: Blacklisted models are clearly marked in model listings  
- **Usage Prevention**: The application prevents you from accidentally using blacklisted models
- **Auto-Discovery**: Benchmark testing automatically discovers and blacklists problematic models
- **Documentation**: Blacklist entries include reasons and timestamps to help track why models were blacklisted

## Performance Benchmarking

The application includes a comprehensive benchmarking system to test model performance:

```bash
# Test all available models with default prompt
./aith test

# Test with custom prompt
./aith test "Explain quantum computing"

# Test specific provider's models
./aith --provider=openrouter test "How are you?"
```

### Benchmark Features

- **Response Time Measurement**: Measures and displays response times in milliseconds
- **Automatic Filtering**: Skips previously blacklisted models during testing
- **Auto-Blacklisting**: Automatically blacklists models that fail or return invalid responses
- **Sorted Results**: Results are sorted by response time for easy comparison
- **Provider-Specific**: Tests models for the currently selected provider

## Architecture

The application follows a modular architecture with clear separation of concerns:

### Core Components

- **SystemUtils**: Handles system-level operations like environment variable access and command execution
- **ConfigManager**: Manages configuration file reading and parsing with priority resolution
- **ProviderManager**: Handles aith provider selection, API key management, and provider-specific settings
- **ModelBlacklist**: Manages model blacklisting with automatic and manual blacklist operations
- **API Module**: Handles HTTP requests and API communications
- **History Module**: Manages chat history storage and retrieval
- **Markdown Module**: Renders aith responses in formatted Markdown
- **Benchmark Module**: Provides performance testing and model comparison functionality

### Configuration Priority

The system resolves configuration values in this order:

1. Environment variables (highest priority)
2. Provider-specific config files (`~/.config/ai/{provider}.conf`)
3. Main configuration file (`~/.config/ai/config`)
4. Built-in defaults (lowest priority)

### Provider Resolution

Provider selection follows this hierarchy:

1. Command line `--provider` flag (highest priority)
2. `AGENT` environment variable
3. `DEFAULT_PROVIDER` from main config file
4. Built-in default ("groq") (lowest priority)

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
