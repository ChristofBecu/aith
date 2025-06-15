#pragma once

#include <string>
#include <memory>

// Forward declarations
class BlacklistFileManager;
class BlacklistParser;

/**
 * @brief Abstract base class for all blacklist operations.
 * 
 * This class defines the common interface that all blacklist operations must implement.
 * It provides shared dependencies (file manager and parser) and enforces a consistent
 * execution pattern for all operations.
 * 
 * All concrete operations (check, add, remove, list) should inherit from this class
 * and implement the pure virtual methods.
 */
class BlacklistOperation {
public:
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~BlacklistOperation() = default;
    
    /**
     * @brief Executes the blacklist operation
     * 
     * This method performs the actual operation logic. It should be implemented
     * by concrete classes to carry out their specific functionality.
     * 
     * @throws std::runtime_error if the operation fails
     */
    virtual void execute() = 0;
    
    /**
     * @brief Validates the inputs for this operation
     * 
     * This method should check that all required inputs are valid before
     * the operation is executed. It should throw an exception if validation fails.
     * 
     * @throws std::invalid_argument if inputs are invalid
     */
    virtual void validateInputs() const = 0;
    
    /**
     * @brief Returns the name of this operation for logging/debugging
     * 
     * @return String name of the operation (e.g., "check", "add", "remove", "list")
     */
    virtual std::string getOperationName() const = 0;

protected:
    /**
     * @brief Protected constructor for base class
     * 
     * @param provider The provider name (may be empty for some operations)
     * @param model The model name (may be empty for some operations)
     * @param fileManager Shared pointer to the file manager
     * @param parser Shared pointer to the parser
     */
    BlacklistOperation(const std::string& provider,
                      const std::string& model,
                      std::shared_ptr<BlacklistFileManager> fileManager,
                      std::shared_ptr<BlacklistParser> parser);
    
    // Common data members accessible to derived classes
    std::string provider_;                                    ///< Provider name
    std::string model_;                                       ///< Model name
    std::shared_ptr<BlacklistFileManager> fileManager_;      ///< File operations
    std::shared_ptr<BlacklistParser> parser_;                ///< Text processing
    
    /**
     * @brief Validates that provider name is not empty and valid
     * @throws std::invalid_argument if provider is invalid
     */
    void validateProvider() const;
    
    /**
     * @brief Validates that model name is not empty and valid
     * @throws std::invalid_argument if model is invalid
     */
    void validateModel() const;
    
    /**
     * @brief Validates that both provider and model are valid
     * @throws std::invalid_argument if either is invalid
     */
    void validateProviderAndModel() const;

private:
    // Non-copyable and non-movable
    BlacklistOperation(const BlacklistOperation&) = delete;
    BlacklistOperation& operator=(const BlacklistOperation&) = delete;
    BlacklistOperation(BlacklistOperation&&) = delete;
    BlacklistOperation& operator=(BlacklistOperation&&) = delete;
};
