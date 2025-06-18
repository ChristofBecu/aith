# AITH - And It Talks, Human

A lightweight C++ terminal application for interacting with AI models from multiple providers.

## Features

- **Multi-Provider Support**: Switch between Groq, OpenRouter, and other AI providers
- **Chat Interface**: Simple command-line chat with AI models  
- **History Management**: View, manage, and reuse conversation history
- **Model Blacklisting**: Automatically avoid problematic models
- **Performance Benchmarking**: Test and compare model response times
- **Markdown Rendering**: Clean formatting of AI responses

## Prerequisites

Developed on Arch Linux. Required dependencies:

```bash
# Arch Linux
sudo pacman -S base-devel cmake ninja
```

## Building

```bash
git clone <repository-url>
cd aith
cmake -B build -G Ninja
ninja -C build
```

The executable will be created as `./build/aith`.

## Configuration

Create configuration files in `~/.config/aith/`:

### Provider Configuration

Create a file for each provider (e.g., `groq.conf`, `openrouter.conf`):

```bash
# ~/.config/aith/groq.conf
API_URL=https://api.groq.com/openai/v1
API_KEY=your-groq-api-key-here
DEFAULT_MODEL=llama-3.1-70b-versatile
```

### Default Provider

```bash
# ~/.config/aith/config  
DEFAULT_PROVIDER=groq
```

### Environment Variables (Alternative)

```bash
export GROQ_API_KEY="your-api-key"
export OPENROUTER_API_KEY="your-openrouter-key"
export AGENT="groq"  # Default provider
```

## Usage

### Basic Chat

```bash
# Start new conversation
./build/aith new "Explain quantum computing"

# Continue conversation  
./build/aith "Can you give me an example?"

# Use specific provider
./build/aith -p openrouter "Write a Python function"
```

### History Management

```bash
# List conversations
./build/aith history

# View conversation
./build/aith history view current
./build/aith history view latest  
./build/aith history view filename.json

# Switch to previous conversation
./build/aith history reuse latest
./build/aith history reuse filename.json
```

### Model Management

```bash
# List available models
./build/aith list

# Test model performance
./build/aith benchmark

# Manage blacklist
./build/aith blacklist list
./build/aith blacklist add groq problematic-model "reason"
./build/aith blacklist remove groq model-name
```

### Help

```bash
./build/aith help
./build/aith --help
```

## Configuration Priority

Settings are resolved in this order (highest to lowest priority):

1. Command line flags (`--provider=name`)
2. Environment variables (`GROQ_API_KEY`)
3. Provider config files (`~/.config/aith/groq.conf`)
4. Main config file (`~/.config/aith/config`)
5. Built-in defaults

## File Locations

- **Configuration**: `~/.config/aith/`
- **History**: `~/aith_histories/`
- **Current conversation**: `~/.config/aith/current_conversation`
- **Blacklist**: `~/.config/aith/blacklist`

## Examples

```bash
# Quick start with Groq
export GROQ_API_KEY="your-key"
./build/aith new "Hello, world!"

# Switch providers mid-conversation
./build/aith -p openrouter "Continue this thought"

# Benchmark all models
./build/aith benchmark "Write a haiku"

# View conversation history
./build/aith history view current | head -20
```

## License

MIT License

Copyright (c) 2025 AITH Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
