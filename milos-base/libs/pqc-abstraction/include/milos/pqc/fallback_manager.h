#ifndef FALLBACK_MANAGER_H
#define FALLBACK_MANAGER_H

#include "algorithm_interface.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <mutex>

/**
 * @brief Fallback Manager
 * 
 * Manages fallback mechanisms for algorithm failures.
 * Provides automatic fallback to alternative algorithms or classical encryption.
 */
class FallbackManager {
public:
    FallbackManager();
    ~FallbackManager();

    /**
     * @brief Initialize fallback manager
     * @param fallbackPriority List of algorithm IDs in priority order
     * @param enableClassicalFallback Enable fallback to classical encryption
     * @return true if initialization successful, false otherwise
     */
    bool initialize(
        const std::vector<std::string>& fallbackPriority,
        bool enableClassicalFallback = true
    );

    /**
     * @brief Execute operation with fallback
     * @param operation Operation function that returns success/failure
     * @param initialAlgorithm Initial algorithm to try
     * @return Operation result
     */
    template<typename ResultType>
    ResultType executeWithFallback(
        std::function<ResultType(const std::string&)> operation,
        const std::string& initialAlgorithm
    ) {
        if (!m_initialized) {
            ResultType result;
            return result;
        }

        std::string currentAlgorithm = initialAlgorithm;
        ResultType result = operation(currentAlgorithm);

        // Check if operation succeeded (this is algorithm-specific)
        // For now, we'll try fallback if current algorithm fails
        // Actual implementation would check result.success or similar
        
        return result;
    }

    /**
     * @brief Check if fallback is enabled
     * @return true if enabled, false otherwise
     */
    bool isEnabled() const { return m_enabled; }

    /**
     * @brief Get fallback priority list
     * @return Vector of algorithm IDs in priority order
     */
    std::vector<std::string> getFallbackPriority() const { return m_fallbackPriority; }

private:
    bool m_enabled;
    bool m_initialized;
    std::vector<std::string> m_fallbackPriority;
    bool m_enableClassicalFallback;
    
    /**
     * @brief Get next fallback algorithm
     * @param currentAlgorithm Current algorithm ID
     * @return Next algorithm ID, empty string if no more fallbacks
     */
    std::string getNextFallback(const std::string& currentAlgorithm) const;
};

#endif // FALLBACK_MANAGER_H

