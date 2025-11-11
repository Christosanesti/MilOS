#include "milos/pqc/algorithm_factory.h"
#include "milos/pqc/algorithm_registry.h"
#include "milos/pqc/algorithm_versioning.h"
#include "kyber_algorithm.h"
#include "dilithium_algorithm.h"
#include "sphincs_algorithm.h"
#include <iostream>

#ifdef HAVE_LIBOQS
#include <oqs/oqs.h>
#endif

/**
 * @brief Register all PQC algorithms with the factory and registry
 * 
 * This function is called during library initialization to register
 * all available PQC algorithms.
 */
void registerPQCAlgorithms() {
#ifdef HAVE_LIBOQS
    AlgorithmFactory& factory = AlgorithmFactory::getInstance();
    AlgorithmRegistry& registry = AlgorithmRegistry::getInstance();
    AlgorithmVersioning& versioning = AlgorithmVersioning::getInstance();

    // Register CRYSTALS-Kyber variants
    std::vector<std::string> kyberVariants = {"Kyber512", "Kyber768", "Kyber1024"};
    for (const auto& variant : kyberVariants) {
        // Register with factory
        factory.registerAlgorithm(variant, [variant]() {
            return std::make_unique<KyberAlgorithm>(variant);
        });

        // Register metadata
        auto kyber = std::make_unique<KyberAlgorithm>(variant);
        if (kyber->isHealthy()) {
            AlgorithmMetadata metadata = kyber->getMetadata();
            registry.registerAlgorithm(metadata);

            // Register version information
            AlgorithmVersion version;
            version.algorithm_id = variant;
            version.version = "1.0.0";
            version.previous_version = "";
            version.is_backward_compatible = true;
            version.migration_path = "No migration needed for initial version";
            versioning.registerVersion(version);
        }
    }

    // Register CRYSTALS-Dilithium variants
    std::vector<std::string> dilithiumVariants = {"Dilithium2", "Dilithium3", "Dilithium5"};
    for (const auto& variant : dilithiumVariants) {
        // Register with factory
        factory.registerAlgorithm(variant, [variant]() {
            return std::make_unique<DilithiumAlgorithm>(variant);
        });

        // Register metadata
        auto dilithium = std::make_unique<DilithiumAlgorithm>(variant);
        if (dilithium->isHealthy()) {
            AlgorithmMetadata metadata = dilithium->getMetadata();
            registry.registerAlgorithm(metadata);

            // Register version information
            AlgorithmVersion version;
            version.algorithm_id = variant;
            version.version = "1.0.0";
            version.previous_version = "";
            version.is_backward_compatible = true;
            version.migration_path = "No migration needed for initial version";
            versioning.registerVersion(version);
        }
    }

    // Register SPHINCS+ variants
    std::vector<std::string> sphincsVariants = {
        "SPHINCS+-SHA256-128f-simple",
        "SPHINCS+-SHA256-128s-simple",
        "SPHINCS+-SHA256-192f-simple",
        "SPHINCS+-SHA256-256f-simple"
    };
    for (const auto& variant : sphincsVariants) {
        // Register with factory
        factory.registerAlgorithm(variant, [variant]() {
            return std::make_unique<SPHINCSAlgorithm>(variant);
        });

        // Register metadata
        auto sphincs = std::make_unique<SPHINCSAlgorithm>(variant);
        if (sphincs->isHealthy()) {
            AlgorithmMetadata metadata = sphincs->getMetadata();
            registry.registerAlgorithm(metadata);

            // Register version information
            AlgorithmVersion version;
            version.algorithm_id = variant;
            version.version = "1.0.0";
            version.previous_version = "";
            version.is_backward_compatible = true;
            version.migration_path = "No migration needed for initial version";
            versioning.registerVersion(version);
        }
    }

    std::cout << "PQC algorithms registered successfully" << std::endl;
#else
    std::cerr << "Warning: liboqs not available. PQC algorithms not registered." << std::endl;
#endif
}

/**
 * @brief Library initialization function
 * 
 * This function is called automatically when the library is loaded.
 * It initializes liboqs and registers all algorithms.
 */
#ifdef __cplusplus
extern "C" {
#endif

__attribute__((constructor))
void pqc_abstraction_init() {
#ifdef HAVE_LIBOQS
    // Initialize liboqs
    OQS_init();
    
    // Register all algorithms
    registerPQCAlgorithms();
#endif
}

#ifdef __cplusplus
}
#endif

