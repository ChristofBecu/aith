#include "model_blacklist.h"
#include "blacklist/blacklist_file_manager.h"
#include "blacklist/blacklist_parser.h"
#include "blacklist/blacklist_operation_factory.h"
#include "blacklist/blacklist_check_operation.h"
#include "blacklist/blacklist_list_operation.h"
#include <iostream>
#include <algorithm>

/**
 * Checks if a model is blacklisted for a specific provider.
 */
bool ModelBlacklist::isModelBlacklisted(const std::string &provider, const std::string &modelName) {
    try {
        // Use the factory to create a BlacklistCheckOperation
        auto checkOperation = BlacklistOperationFactory::createOperation(
            BlacklistOperationFactory::OperationType::CHECK, provider, modelName);
        
        // Execute the check
        checkOperation->execute();
        
        // Cast to BlacklistCheckOperation to get the result
        auto* checkOp = dynamic_cast<BlacklistCheckOperation*>(checkOperation.get());
        if (checkOp) {
            return checkOp->isBlacklisted();
        }
        
        // Fallback - should not happen in normal operation
        std::cerr << "Warning: Failed to cast to BlacklistCheckOperation" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        // Log error but don't crash - blacklist check should be resilient
        std::cerr << "Error checking blacklist for " << provider << "/" << modelName 
                  << ": " << e.what() << std::endl;
        return false;
    }
}

/**
 * Adds a model to the blacklist for a specific provider.
 */
void ModelBlacklist::addModelToBlacklist(const std::string &provider, const std::string &modelName, 
                                        const std::string &reason) {
    try {
        // Use the factory to create a BlacklistAddOperation
        auto addOperation = BlacklistOperationFactory::createOperation(
            BlacklistOperationFactory::OperationType::ADD, provider, modelName, reason);
        
        // Execute the add operation
        addOperation->execute();
        
    } catch (const std::exception& e) {
        // Log error but don't crash - add operation should handle most errors internally
        std::cerr << "Error in addModelToBlacklist: " << e.what() << std::endl;
    }
}

/**
 * Removes a model from the blacklist for a specific provider.
 */
void ModelBlacklist::removeModelFromBlacklist(const std::string &provider, const std::string &modelName) {
    try {
        // Use the factory to create a BlacklistRemoveOperation
        auto removeOperation = BlacklistOperationFactory::createOperation(
            BlacklistOperationFactory::OperationType::REMOVE, provider, modelName);
        
        // Execute the remove operation
        removeOperation->execute();
        
    } catch (const std::exception& e) {
        // Log error but don't crash - remove operation should handle most errors internally
        std::cerr << "Error in removeModelFromBlacklist: " << e.what() << std::endl;
    }
}

/**
 * Returns a list of all blacklisted models and their information.
 */
std::vector<BlacklistEntry> ModelBlacklist::getBlacklistedModels() {
    try {
        // Use the factory to create a BlacklistListOperation
        auto listOperation = BlacklistOperationFactory::createOperation(
            BlacklistOperationFactory::OperationType::LIST);
        
        // Execute the list operation
        listOperation->execute();
        
        // Cast to BlacklistListOperation to get the results
        auto* listOp = dynamic_cast<BlacklistListOperation*>(listOperation.get());
        if (listOp) {
            return listOp->getBlacklistedModels();
        }
        
        // Fallback - should not happen in normal operation
        std::cerr << "Warning: Failed to cast to BlacklistListOperation" << std::endl;
        return std::vector<BlacklistEntry>();
        
    } catch (const std::exception& e) {
        // Log error but don't crash - list operation should be resilient
        std::cerr << "Error in getBlacklistedModels: " << e.what() << std::endl;
        return std::vector<BlacklistEntry>();
    }
}
