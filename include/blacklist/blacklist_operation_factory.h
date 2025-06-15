#pragma once

#include <string>
#include <memory>

// Forward declarations
class BlacklistOperation;
class BlacklistFileManager;
class BlacklistParser;

/**
 * @brief Factory class for creating blacklist operations.
 * 
 * This factory provides a centralized way to create different types of blacklist
 * operations (check, add, remove, list) with proper validation and dependency injection.
 * It encapsulates the creation logic and ensures all operations are properly configured.
 */
class BlacklistOperationFactory {
public:
    /**
     * @brief Enumeration of available operation types
     */
    enum class OperationType {
        CHECK,      ///< Check if a model is blacklisted
        ADD,        ///< Add a model to the blacklist
        REMOVE,     ///< Remove a model from the blacklist
        LIST        ///< List all blacklisted models
    };
    
    /**
     * @brief Creates a blacklist operation of the specified type
     * 
     * @param type The type of operation to create
     * @param provider The provider name (required for CHECK, ADD, REMOVE)
     * @param model The model name (required for CHECK, ADD, REMOVE)
     * @param reason The reason for blacklisting (optional, used for ADD)
     * @return Unique pointer to the created operation
     * @throws std::invalid_argument if parameters are invalid for the operation type
     * @throws std::runtime_error if operation creation fails
     */
    static std::unique_ptr<BlacklistOperation> createOperation(
        OperationType type,
        const std::string& provider = "",
        const std::string& model = "",
        const std::string& reason = ""
    );
    
    /**
     * @brief Creates a blacklist operation with shared dependencies
     * 
     * This overload allows for dependency injection of custom file manager and parser.
     * Useful for testing or when custom implementations are needed.
     * 
     * @param type The type of operation to create
     * @param fileManager Shared pointer to the file manager
     * @param parser Shared pointer to the parser
     * @param provider The provider name (required for CHECK, ADD, REMOVE)
     * @param model The model name (required for CHECK, ADD, REMOVE)
     * @param reason The reason for blacklisting (optional, used for ADD)
     * @return Unique pointer to the created operation
     * @throws std::invalid_argument if parameters are invalid for the operation type
     * @throws std::runtime_error if operation creation fails
     */
    static std::unique_ptr<BlacklistOperation> createOperation(
        OperationType type,
        std::shared_ptr<BlacklistFileManager> fileManager,
        std::shared_ptr<BlacklistParser> parser,
        const std::string& provider = "",
        const std::string& model = "",
        const std::string& reason = ""
    );
    
    /**
     * @brief Converts operation type enum to string for logging/debugging
     * 
     * @param type The operation type to convert
     * @return String representation of the operation type
     */
    static std::string operationTypeToString(OperationType type);
    
    /**
     * @brief Converts string to operation type enum
     * 
     * @param typeString The string representation of the operation type
     * @return The corresponding OperationType enum value
     * @throws std::invalid_argument if the string doesn't match any operation type
     */
    static OperationType stringToOperationType(const std::string& typeString);

private:
    /**
     * @brief Validates that the operation type is supported
     * 
     * @param type The operation type to validate
     * @throws std::invalid_argument if the operation type is not supported
     */
    static void validateOperationType(OperationType type);
    
    /**
     * @brief Validates parameters for a specific operation type
     * 
     * @param type The operation type
     * @param provider The provider name
     * @param model The model name
     * @throws std::invalid_argument if parameters are invalid for the operation type
     */
    static void validateParameters(OperationType type, 
                                 const std::string& provider,
                                 const std::string& model);
    
    /**
     * @brief Gets the default file manager instance
     * 
     * @return Shared pointer to the default BlacklistFileManager
     */
    static std::shared_ptr<BlacklistFileManager> getDefaultFileManager();
    
    /**
     * @brief Gets the default parser instance
     * 
     * @return Shared pointer to the default BlacklistParser
     */
    static std::shared_ptr<BlacklistParser> getDefaultParser();
    
    /**
     * @brief Creates the concrete operation instance
     * 
     * This method will be updated as concrete operation classes are implemented
     * in subsequent phases.
     * 
     * @param type The operation type
     * @param fileManager The file manager dependency
     * @param parser The parser dependency
     * @param provider The provider name
     * @param model The model name
     * @param reason The reason for blacklisting
     * @return Unique pointer to the created operation
     */
    static std::unique_ptr<BlacklistOperation> createConcreteOperation(
        OperationType type,
        std::shared_ptr<BlacklistFileManager> fileManager,
        std::shared_ptr<BlacklistParser> parser,
        const std::string& provider,
        const std::string& model,
        const std::string& reason
    );
};
