#include "blacklist_operation.h"
#include "blacklist_file_manager.h"
#include "blacklist_parser.h"
#include <stdexcept>

/**
 * Protected constructor for base class.
 */
BlacklistOperation::BlacklistOperation(const std::string& provider,
                                      const std::string& model,
                                      std::shared_ptr<BlacklistFileManager> fileManager,
                                      std::shared_ptr<BlacklistParser> parser)
    : provider_(provider)
    , model_(model)
    , fileManager_(fileManager)
    , parser_(parser) {
    
    // Validate that required dependencies are provided
    if (!fileManager_) {
        throw std::invalid_argument("BlacklistFileManager cannot be null");
    }
    
    if (!parser_) {
        throw std::invalid_argument("BlacklistParser cannot be null");
    }
}

/**
 * Validates that provider name is not empty and valid.
 */
void BlacklistOperation::validateProvider() const {
    if (provider_.empty()) {
        throw std::invalid_argument("Provider name cannot be empty");
    }
    
    if (!BlacklistParser::isValidProvider(provider_)) {
        throw std::invalid_argument("Provider name contains invalid characters: " + provider_);
    }
}

/**
 * Validates that model name is not empty and valid.
 */
void BlacklistOperation::validateModel() const {
    if (model_.empty()) {
        throw std::invalid_argument("Model name cannot be empty");
    }
    
    if (!BlacklistParser::isValidModel(model_)) {
        throw std::invalid_argument("Model name contains invalid characters: " + model_);
    }
}

/**
 * Validates that both provider and model are valid.
 */
void BlacklistOperation::validateProviderAndModel() const {
    validateProvider();
    validateModel();
}
