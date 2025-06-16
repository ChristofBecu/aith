#include "json_file_handler.h"
#include <fstream>
#include <stdexcept>
#include <memory>

Json::Value JsonFileHandler::read(const std::string& filePath) {
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open JSON file for reading: " + filePath);
    }
    
    Json::Value json;
    Json::CharReaderBuilder builder;
    std::string errors;
    
    if (!Json::parseFromStream(builder, file, &json, &errors)) {
        throw std::runtime_error("Failed to parse JSON file: " + filePath + " - " + errors);
    }
    
    return json;
}

void JsonFileHandler::write(const std::string& filePath, const Json::Value& json) {
    std::ofstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open JSON file for writing: " + filePath);
    }
    
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(json, &file);
}
