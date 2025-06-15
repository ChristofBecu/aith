#pragma once

#include "blacklist_operation.h"
#include "blacklist_parser.h"
#include <string>
#include <memory>

// Forward declarations
class BlacklistFileManager;

/**
 * @brief Operation for checking if a model is blacklisted.
 * 
 * This operation checks whether a specific model from a provider is present
 * in the blacklist. It reads the blacklist file, parses each entry, and
 * returns the result of the check.
 * 
 * Usage:
 * ```cpp
 * auto checkOp = std::make_unique<BlacklistCheckOperation>(
 *     "openrouter", "gpt-4", fileManager, parser);
 * checkOp->execute();
 * bool isBlacklisted = checkOp->isBlacklisted();
 * ```
 */
class BlacklistCheckOperation : public BlacklistOperation {
public:
    /**
     * @brief Constructs a blacklist check operation
     * 
     * @param provider The provider name to check
     * @param model The model name to check
     * @param fileManager Shared pointer to the file manager
     * @param parser Shared pointer to the parser
     */
    BlacklistCheckOperation(const std::string& provider, 
                           const std::string& model,
                           std::shared_ptr<BlacklistFileManager> fileManager,
                           std::shared_ptr<BlacklistParser> parser);
    
    /**
     * @brief Destructor
     */
    virtual ~BlacklistCheckOperation() = default;
    
    /**
     * @brief Executes the blacklist check operation
     * 
     * This method performs the actual check by reading the blacklist file,
     * parsing each entry, and determining if the provider/model combination
     * is present in the blacklist.
     * 
     * @throws std::runtime_error if file operations fail critically
     */
    void execute() override;
    
    /**
     * @brief Validates the inputs for this operation
     * 
     * Checks that both provider and model names are valid and non-empty.
     * 
     * @throws std::invalid_argument if inputs are invalid
     */
    void validateInputs() const override;
    
    /**
     * @brief Returns the name of this operation
     * 
     * @return "check"
     */
    std::string getOperationName() const override;
    
    /**
     * @brief Returns the result of the blacklist check
     * 
     * This method should only be called after execute() has been called.
     * 
     * @return True if the model is blacklisted, false otherwise
     * @throws std::logic_error if execute() has not been called yet
     */
    bool isBlacklisted() const;
    
    /**
     * @brief Checks if the operation has been executed
     * 
     * @return True if execute() has been called, false otherwise
     */
    bool hasBeenExecuted() const;

private:
    bool result_;           ///< The result of the blacklist check
    bool executed_;         ///< Whether execute() has been called
    
    /**
     * @brief Performs the actual blacklist check logic
     * 
     * This method contains the core logic for checking if a model is blacklisted.
     * It handles file reading, line parsing, and matching logic.
     * 
     * @return True if the model is blacklisted, false otherwise
     */
    bool performCheck();
    
    /**
     * @brief Checks if a parsed entry matches the target provider/model
     * 
     * @param entry The parsed blacklist entry to check
     * @return True if the entry matches, false otherwise
     */
    bool entryMatches(const ParsedBlacklistEntry& entry) const;
};
