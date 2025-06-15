#pragma once

#include "blacklist_operation.h"
#include <string>
#include <memory>

// Forward declarations
class BlacklistFileManager;
class BlacklistParser;

/**
 * @brief Operation for adding models to the blacklist.
 * 
 * This operation handles the complete process of adding a model to the blacklist,
 * including duplicate checking, entry formatting, and file writing. It provides
 * proper error handling and user feedback.
 */
class BlacklistAddOperation : public BlacklistOperation {
public:
    /**
     * @brief Constructs a blacklist add operation.
     * 
     * @param provider The provider name for the model to add
     * @param model The model name to add to the blacklist
     * @param reason The reason for blacklisting this model
     * @param fileManager Shared pointer to the file manager for I/O operations
     * @param parser Shared pointer to the parser for entry formatting and validation
     */
    BlacklistAddOperation(const std::string& provider, 
                         const std::string& model,
                         const std::string& reason,
                         std::shared_ptr<BlacklistFileManager> fileManager,
                         std::shared_ptr<BlacklistParser> parser);

    /**
     * @brief Executes the add operation.
     * 
     * Performs the complete add process:
     * 1. Validates inputs
     * 2. Checks if model is already blacklisted
     * 3. Formats the blacklist entry
     * 4. Appends to the blacklist file
     * 5. Provides user feedback
     */
    void execute() override;

    /**
     * @brief Validates inputs specific to add operations.
     * 
     * Ensures provider, model, and reason are valid for adding to blacklist.
     */
    void validateInputs() const override;

    /**
     * @brief Returns the name of this operation.
     * 
     * @return String identifier for this operation type
     */
    std::string getOperationName() const override;

    /**
     * @brief Checks if the operation completed successfully.
     * 
     * @return True if the model was successfully added, false otherwise
     */
    bool wasSuccessful() const;

    /**
     * @brief Checks if the operation has been executed.
     * 
     * @return True if execute() has been called, false otherwise
     */
    bool hasBeenExecuted() const;

private:
    std::string reason_;        ///< The reason for blacklisting
    bool successful_;           ///< Whether the operation succeeded
    bool executed_;             ///< Whether the operation has been executed

    /**
     * @brief Checks if the model is already blacklisted.
     * 
     * Uses a check operation to determine if the model is already in the blacklist.
     * 
     * @return True if already blacklisted, false otherwise
     */
    bool isAlreadyBlacklisted();

    /**
     * @brief Performs the actual add operation.
     * 
     * Formats the entry and appends it to the blacklist file.
     */
    void performAdd();

    /**
     * @brief Validates that the reason is not empty and meaningful.
     */
    void validateReason() const;
};
