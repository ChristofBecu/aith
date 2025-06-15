#pragma once

#include "blacklist_operation.h"
#include "blacklist_parser.h"
#include <string>
#include <memory>
#include <vector>

// Forward declarations
class BlacklistFileManager;

/**
 * @brief Operation for removing models from the blacklist.
 * 
 * This operation handles the complete process of removing a model from the blacklist,
 * including file reading, line filtering, and file rewriting. It provides proper
 * error handling and user feedback for both success and failure cases.
 */
class BlacklistRemoveOperation : public BlacklistOperation {
public:
    /**
     * @brief Constructs a blacklist remove operation.
     * 
     * @param provider The provider name for the model to remove
     * @param model The model name to remove from the blacklist
     * @param fileManager Shared pointer to the file manager for I/O operations
     * @param parser Shared pointer to the parser for entry parsing and validation
     */
    BlacklistRemoveOperation(const std::string& provider,
                           const std::string& model,
                           std::shared_ptr<BlacklistFileManager> fileManager,
                           std::shared_ptr<BlacklistParser> parser);

    /**
     * @brief Executes the remove operation.
     * 
     * Performs the complete remove process:
     * 1. Validates inputs
     * 2. Checks if blacklist file exists
     * 3. Reads all lines from the blacklist
     * 4. Filters out the target entry
     * 5. Rewrites the blacklist file
     * 6. Provides user feedback
     */
    void execute() override;

    /**
     * @brief Validates inputs specific to remove operations.
     * 
     * Ensures provider and model are valid for removing from blacklist.
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
     * @return True if the model was successfully removed, false otherwise
     */
    bool wasSuccessful() const;

    /**
     * @brief Checks if the operation has been executed.
     * 
     * @return True if execute() has been called, false otherwise
     */
    bool hasBeenExecuted() const;

    /**
     * @brief Checks if the target model was found in the blacklist.
     * 
     * Only valid after execute() has been called.
     * 
     * @return True if the model was found (regardless of removal success), false otherwise
     */
    bool wasModelFound() const;

private:
    bool successful_;           ///< Whether the operation succeeded
    bool executed_;             ///< Whether the operation has been executed
    bool modelFound_;           ///< Whether the target model was found

    /**
     * @brief Checks if the blacklist file exists.
     * 
     * @return True if file exists, false otherwise
     */
    bool checkFileExists();

    /**
     * @brief Performs the actual remove operation.
     * 
     * Reads the file, filters out the target entry, and rewrites it.
     */
    void performRemove();

    /**
     * @brief Filters out the target entry from the given lines.
     * 
     * @param lines The original lines from the blacklist file
     * @return Vector of lines with the target entry removed
     */
    std::vector<std::string> filterOutTargetEntry(const std::vector<std::string>& lines);

    /**
     * @brief Checks if a parsed entry matches the target provider/model.
     * 
     * @param entry The parsed blacklist entry to check
     * @return True if it matches the target, false otherwise
     */
    bool isTargetEntry(const ParsedBlacklistEntry& entry) const;
};
