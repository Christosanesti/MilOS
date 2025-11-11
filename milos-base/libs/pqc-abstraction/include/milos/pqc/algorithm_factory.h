#ifndef ALGORITHM_FACTORY_H
#define ALGORITHM_FACTORY_H

#include "algorithm_interface.h"
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <mutex>

/**
 * @brief Algorithm Factory
 * 
 * Factory pattern for creating algorithm instances at runtime.
 * Supports algorithm registration, discovery, and instantiation.
 */
class AlgorithmFactory {
public:
    /**
     * @brief Algorithm creator function type
     */
    using AlgorithmCreator = std::function<std::unique_ptr<IAlgorithm>()>;

    /**
     * @brief Get singleton instance
     * @return Factory instance
     */
    static AlgorithmFactory& getInstance();

    /**
     * @brief Register algorithm creator
     * @param algorithmId Algorithm ID
     * @param creator Creator function
     * @return true if registration successful, false otherwise
     */
    bool registerAlgorithm(const std::string& algorithmId, AlgorithmCreator creator);

    /**
     * @brief Create algorithm instance
     * @param algorithmId Algorithm ID
     * @return Algorithm instance, nullptr if not found
     */
    std::unique_ptr<IAlgorithm> createAlgorithm(const std::string& algorithmId);

    /**
     * @brief Get list of registered algorithm IDs
     * @return Vector of algorithm IDs
     */
    std::vector<std::string> getRegisteredAlgorithms() const;

    /**
     * @brief Check if algorithm is registered
     * @param algorithmId Algorithm ID
     * @return true if registered, false otherwise
     */
    bool isAlgorithmRegistered(const std::string& algorithmId) const;

    /**
     * @brief Unregister algorithm
     * @param algorithmId Algorithm ID
     * @return true if unregistration successful, false otherwise
     */
    bool unregisterAlgorithm(const std::string& algorithmId);

private:
    AlgorithmFactory() = default;
    ~AlgorithmFactory() = default;
    AlgorithmFactory(const AlgorithmFactory&) = delete;
    AlgorithmFactory& operator=(const AlgorithmFactory&) = delete;

    std::map<std::string, AlgorithmCreator> m_creators;
    mutable std::mutex m_mutex;
};

#endif // ALGORITHM_FACTORY_H

