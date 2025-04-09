#include <iostream>
#include <filesystem>
#include <string>
#include "utils.h"
#include "api.h"
#include "history.h"
#include "markdown.h"

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
        chat(prompt, argc > 3 ? argv[3] : "", apiKey, currentHistory, true);
    } else {
        std::string prompt = argv[1];
        chat(prompt, argc > 2 ? argv[2] : "", apiKey, currentHistory, false);
    }

    return 0;
}