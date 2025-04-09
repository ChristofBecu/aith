#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <json/json.h> // Assuming JsonCpp library is used
#include <filesystem>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <algorithm> // For std::replace and std::remove_if
#include <cctype>    // For std::isalnum
#include <functional> // For std::function
#include <regex>
#include <md4c.h>

const std::string GROQ_API_URL = "https://api.groq.com/openai/v1";
const std::string DEFAULT_MODEL = "llama-3.3-70b-specdec";

std::string getEnvVar(const std::string &key) {
    const char *val = std::getenv(key.c_str());
    if (val == nullptr) {
        return "";
    }
    return std::string(val);
}

void executeCommand(const std::string &command) {
    int ret_code = std::system(command.c_str());
    if (ret_code != 0) {
        std::cerr << "Command failed: " << command << std::endl;
        std::exit(1);
    }
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, std::function<void(FILE*)>> pipe(popen(cmd, "r"), [](FILE* f) { pclose(f); });
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void listModels(const std::string &apiKey) {
    std::string command = "curl -s -X GET " + GROQ_API_URL + "/models -H 'Authorization: Bearer " + apiKey + "'";
    std::string result = exec(command.c_str());

    Json::Value data;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(result);
    if (!Json::parseFromStream(reader, s, &data, &errs)) {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        return;
    }

    for (const auto &model : data["data"]) {
        std::time_t created = model["created"].asInt64();
        std::string date = std::asctime(std::localtime(&created));
        std::cout << model["id"].asString() << " | Owner: " << model["owned_by"].asString() << " | Created: " << date;
    }
}

void startNewHistory(const std::string &prompt, const std::string &historyDir, const std::string &currentHistory) {
    if (std::filesystem::exists(currentHistory)) {
        std::time_t now = std::time(nullptr);
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&now));
        std::string title = prompt;
        std::replace(title.begin(), title.end(), ' ', '_');
        title.erase(std::remove_if(title.begin(), title.end(), [](char c) { return !std::isalnum(c) && c != '_'; }), title.end());
        std::filesystem::rename(currentHistory, historyDir + "/history_" + title + "_" + timestamp + ".json");
    }

    std::ofstream file(currentHistory);
    file << "[]";
    file.close();

    Json::Value history(Json::arrayValue);
    Json::Value entry;
    entry["role"] = "user";
    entry["content"] = prompt;
    history.append(entry);

    std::ofstream historyFile(currentHistory);
    historyFile << history;
    historyFile.close();
}

void addToHistory(const std::string &role, const std::string &content, const std::string &currentHistory) {
    std::ifstream file(currentHistory);
    Json::Value history;
    file >> history;
    file.close();

    Json::Value entry;
    entry["role"] = role;
    entry["content"] = content;
    history.append(entry);

    std::ofstream historyFile(currentHistory);
    historyFile << history;
    historyFile.close();
}

void renderMarkdown(const std::string &markdown) {
    // Write the Markdown content to a temporary file
    std::string tempFile = "/tmp/markdown.md";
    std::ofstream outFile(tempFile);
    if (!outFile) {
        std::cerr << "Failed to create temporary file for Markdown rendering." << std::endl;
        return;
    }
    outFile << markdown;
    outFile.close();

    // Use mdcat to render the Markdown in the terminal
    std::string command = "mdcat " + tempFile;
    int ret_code = std::system(command.c_str());
    if (ret_code != 0) {
        std::cerr << "Failed to render Markdown with mdcat." << std::endl;
    }

    // Optionally, clean up the temporary file
    std::remove(tempFile.c_str());
}

void chat(const std::string &prompt, const std::string &model, const std::string &apiKey, const std::string &currentHistory, bool newChat) {
    std::string selectedModel = model.empty() ? DEFAULT_MODEL : model;

    // Read the default prompt from $HOME/.config/defaultprompt
    std::string home = getEnvVar("HOME");
    std::string defaultPromptPath = home + "/.config/defaultprompt";
    std::string defaultPrompt;
    if (std::filesystem::exists(defaultPromptPath)) {
        std::ifstream defaultPromptFile(defaultPromptPath);
        std::ostringstream buffer;
        buffer << defaultPromptFile.rdbuf();
        defaultPrompt = buffer.str();
        defaultPromptFile.close();
    }

    if (!std::filesystem::exists(currentHistory)) {
        std::ofstream file(currentHistory);
        file << "[]";
        file.close();
    }

    if (!newChat) {
        addToHistory("user", prompt, currentHistory);
    }

    std::ifstream file(currentHistory);
    Json::Value history;
    file >> history;
    file.close();

    // Add the default prompt as a system message if it exists
    if (!defaultPrompt.empty()) {
        Json::Value systemMessage;
        systemMessage["role"] = "system";
        systemMessage["content"] = defaultPrompt;

        // Create a new array with the system message prepended
        Json::Value newHistory(Json::arrayValue);
        newHistory.append(systemMessage);
        for (const auto &message : history) {
            newHistory.append(message);
        }
        history = newHistory;
    }

    Json::Value payload;
    payload["model"] = selectedModel;
    payload["messages"] = history;

    Json::StreamWriterBuilder writer;
    std::string payloadJson = Json::writeString(writer, payload);

    // Write the JSON payload to a temporary file
    std::string tempFile = "/tmp/payload.json";
    std::ofstream outFile(tempFile);
    outFile << payloadJson;
    outFile.close();

    // Use the temporary file with curl
    std::string command = "curl -s -X POST " + GROQ_API_URL + "/chat/completions -H 'Authorization: Bearer " + apiKey + "' -H 'Content-Type: application/json' -d @" + tempFile;
    std::string response = exec(command.c_str());

    Json::Value data;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(response);
    if (!Json::parseFromStream(reader, s, &data, &errs)) {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        return;
    }

    if (!data.isMember("choices") || !data["choices"][0].isMember("message")) {
        std::cerr << "Invalid API response received:" << std::endl;
        std::cerr << response << std::endl;
        return;
    }

    std::string answer = data["choices"][0]["message"]["content"].asString();

    // Render Markdown output
    // std::cout << "Rendered Markdown Output:" << std::endl;
    renderMarkdown(answer);

    addToHistory("assistant", answer, currentHistory);
}

int main(int argc, char *argv[]) {
    std::string apiKey = getEnvVar("GROQ_API_KEY");
    if (apiKey.empty()) {
        std::cerr << "Fout: GROQ_API_KEY is niet ingesteld." << std::endl;
        return 1;
    }

    std::string home = getEnvVar("HOME");
    std::string historyDir = home + "/ai_histories";
    std::string currentHistory = historyDir + "/current_history.json";

    std::filesystem::create_directories(historyDir);

    if (argc < 2) {
        std::cerr << "Gebruik: ./ai [list | history | new \"prompt\" | \"prompt\"] [model (optioneel)]" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    if (command == "list") {
        std::cout << "Beschikbare modellen:" << std::endl;
        listModels(apiKey);
    } else if (command == "history") {
        for (const auto &entry : std::filesystem::directory_iterator(historyDir)) {
            std::cout << entry.path().filename().string() << std::endl;
        }
    } else if (command == "new") {
        if (argc < 3) {
            std::cerr << "Gebruik: ./ai new \"prompt\"" << std::endl;
            return 1;
        }
        std::string prompt = argv[2];
        startNewHistory(prompt, historyDir, currentHistory);
        // std::cout << "Nieuwe geschiedenis gestart met prompt: \"" << prompt << "\"" << std::endl;
        chat(prompt, argc > 3 ? argv[3] : "", apiKey, currentHistory, true);
    } else {
        std::string prompt = argv[1];
        chat(prompt, argc > 2 ? argv[2] : "", apiKey, currentHistory, false);
    }

    return 0;
}