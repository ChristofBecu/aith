#include "blacklist_operation_factory.h"
#include "blacklist_operation.h"
#include "blacklist_check_operation.h"
#include "blacklist_add_operation.h"
#include "blacklist_remove_operation.h"
#include "blacklist_list_operation.h"
#include "blacklist_file_manager.h"
#include "blacklist_parser.h"
#include <stdexcept>
#include <algorithm>

/**
 * Creates a blacklist operation of the specified type with default dependencies.
 */
std::unique_ptr<BlacklistOperation> BlacklistOperationFactory::createOperation(
    OperationType type,
    const std::string& provider,
    const std::string& model,
    const std::string& reason) {
    
    // Get default dependencies
    auto fileManager = getDefaultFileManager();
    auto parser = getDefaultParser();
    
    return createOperation(type, fileManager, parser, provider, model, reason);
}

/**
 * Creates a blacklist operation with custom dependencies.
 */
std::unique_ptr<BlacklistOperation> BlacklistOperationFactory::createOperation(
    OperationType type,
    std::shared_ptr<BlacklistFileManager> fileManager,
    std::shared_ptr<BlacklistParser> parser,
    const std::string& provider,
    const std::string& model,
    const std::string& reason) {
    
    // Validate inputs
    validateOperationType(type);
    validateParameters(type, provider, model);
    
    if (!fileManager) {
        throw std::invalid_argument("BlacklistFileManager cannot be null");
    }
    
    if (!parser) {
        throw std::invalid_argument("BlacklistParser cannot be null");
    }
    
    // Create the concrete operation
    return createConcreteOperation(type, fileManager, parser, provider, model, reason);
}

/**
 * Converts operation type enum to string for logging/debugging.
 */
std::string BlacklistOperationFactory::operationTypeToString(OperationType type) {
    switch (type) {
        case OperationType::CHECK:
            return "check";
        case OperationType::ADD:
            return "add";
        case OperationType::REMOVE:
            return "remove";
        case OperationType::LIST:
            return "list";
        default:
            throw std::invalid_argument("Unknown operation type");
    }
}

/**
 * Converts string to operation type enum.
 */
BlacklistOperationFactory::OperationType BlacklistOperationFactory::stringToOperationType(
    const std::string& typeString) {
    
    std::string lowerType = typeString;
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    
    if (lowerType == "check") {
        return OperationType::CHECK;
    } else if (lowerType == "add") {
        return OperationType::ADD;
    } else if (lowerType == "remove") {
        return OperationType::REMOVE;
    } else if (lowerType == "list") {
        return OperationType::LIST;
    } else {
        throw std::invalid_argument("Unknown operation type: " + typeString);
    }
}

/**
 * Validates that the operation type is supported.
 */
void BlacklistOperationFactory::validateOperationType(OperationType type) {
    switch (type) {
        case OperationType::CHECK:
        case OperationType::ADD:
        case OperationType::REMOVE:
        case OperationType::LIST:
            // All supported operation types
            break;
        default:
            throw std::invalid_argument("Unsupported operation type");
    }
}

/**
 * Validates parameters for a specific operation type.
 */
void BlacklistOperationFactory::validateParameters(OperationType type, 
                                                  const std::string& provider,
                                                  const std::string& model) {
    switch (type) {
        case OperationType::CHECK:
        case OperationType::ADD:
        case OperationType::REMOVE:
            // These operations require both provider and model
            if (provider.empty()) {
                throw std::invalid_argument("Provider name is required for " + 
                                          operationTypeToString(type) + " operation");
            }
            if (model.empty()) {
                throw std::invalid_argument("Model name is required for " + 
                                          operationTypeToString(type) + " operation");
            }
            
            // Use parser validation methods
            if (!BlacklistParser::isValidProvider(provider)) {
                throw std::invalid_argument("Invalid provider name: " + provider);
            }
            if (!BlacklistParser::isValidModel(model)) {
                throw std::invalid_argument("Invalid model name: " + model);
            }
            break;
            
        case OperationType::LIST:
            // LIST operation doesn't require provider or model
            // (provider and model parameters are ignored for LIST)
            break;
            
        default:
            throw std::invalid_argument("Unknown operation type for parameter validation");
    }
}

/**
 * Gets the default file manager instance.
 */
std::shared_ptr<BlacklistFileManager> BlacklistOperationFactory::getDefaultFileManager() {
    // For now, create a new instance each time
    // In future optimizations, this could be changed to a singleton pattern
    return std::make_shared<BlacklistFileManager>();
}

/**
 * Gets the default parser instance.
 */
std::shared_ptr<BlacklistParser> BlacklistOperationFactory::getDefaultParser() {
    // BlacklistParser is stateless with only static methods,
    // but we create a shared_ptr for consistency with the interface
    return std::make_shared<BlacklistParser>();
}

/**
 * Creates the concrete operation instance.
 * 
 * NOTE: This method currently throws "not implemented" exceptions since
 * concrete operation classes haven't been created yet. This will be updated
 * in subsequent phases as each operation class is implemented.
 */
std::unique_ptr<BlacklistOperation> BlacklistOperationFactory::createConcreteOperation(
    OperationType type,
    std::shared_ptr<BlacklistFileManager> fileManager,
    std::shared_ptr<BlacklistParser> parser,
    const std::string& provider,
    const std::string& model,
    const std::string& reason) {
    
    switch (type) {
        case OperationType::CHECK:
            // Create and return BlacklistCheckOperation
            return std::make_unique<BlacklistCheckOperation>(provider, model, fileManager, parser);
            
        case OperationType::ADD:
            // Create and return BlacklistAddOperation
            return std::make_unique<BlacklistAddOperation>(provider, model, reason, fileManager, parser);
            
        case OperationType::REMOVE:
            // Create and return BlacklistRemoveOperation
            return std::make_unique<BlacklistRemoveOperation>(provider, model, fileManager, parser);
            
        case OperationType::LIST:
            // Create and return BlacklistListOperation
            return std::make_unique<BlacklistListOperation>(fileManager, parser);
            
        default:
            throw std::invalid_argument("Unknown operation type for creation");
    }
}
