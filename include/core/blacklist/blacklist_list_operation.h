#pragma once

#include "blacklist_operation.h"
#include "../model_blacklist.h"  // For BlacklistEntry
#include <vector>

/**
 * @brief Concrete operation for listing all blacklisted models.
 * 
 * This operation retrieves all blacklisted models from the blacklist file,
 * parses each entry, and returns them as a vector of BlacklistEntry structures.
 * It handles file reading errors gracefully and returns an empty list if
 * the blacklist file doesn't exist.
 */
class BlacklistListOperation : public BlacklistOperation {
public:
    /**
     * @brief Constructs a BlacklistListOperation with required dependencies.
     * 
     * @param fileManager Shared pointer to the file manager for file operations
     * @param parser Shared pointer to the parser for line parsing
     */
    BlacklistListOperation(std::shared_ptr<BlacklistFileManager> fileManager, 
                          std::shared_ptr<BlacklistParser> parser);
    
    /**
     * @brief Executes the list operation.
     * 
     * Reads all lines from the blacklist file, parses each entry,
     * and stores the results internally. The results can be retrieved
     * using getBlacklistedModels().
     * 
     * @throws std::runtime_error if file reading fails unexpectedly
     */
    void execute() override;
    
    /**
     * @brief Gets the list of blacklisted models after execution.
     * 
     * @return Vector of BlacklistEntry structures containing all blacklisted models
     * @note This method should only be called after execute() has been called
     */
    std::vector<BlacklistEntry> getBlacklistedModels() const;

protected:
    /**
     * @brief Validates inputs for the list operation.
     * 
     * The list operation doesn't require any specific inputs,
     * so this method always succeeds.
     */
    void validateInputs() const override;
    
    /**
     * @brief Gets the operation name for logging and debugging.
     * 
     * @return "list" as the operation name
     */
    std::string getOperationName() const override;

private:
    std::vector<BlacklistEntry> blacklistedModels_;  ///< Storage for the retrieved models
};
