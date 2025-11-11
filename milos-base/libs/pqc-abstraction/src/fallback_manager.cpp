#include "milos/pqc/fallback_manager.h"
#include <algorithm>

FallbackManager::FallbackManager()
    : m_enabled(false)
    , m_initialized(false)
    , m_enableClassicalFallback(true)
{
}

FallbackManager::~FallbackManager() {
}

bool FallbackManager::initialize(
    const std::vector<std::string>& fallbackPriority,
    bool enableClassicalFallback
) {
    if (fallbackPriority.empty()) {
        return false;
    }

    m_fallbackPriority = fallbackPriority;
    m_enableClassicalFallback = enableClassicalFallback;
    m_enabled = true;
    m_initialized = true;
    
    return true;
}

std::string FallbackManager::getNextFallback(const std::string& currentAlgorithm) const {
    if (!m_initialized) {
        return "";
    }

    auto it = std::find(m_fallbackPriority.begin(), m_fallbackPriority.end(), currentAlgorithm);
    if (it == m_fallbackPriority.end()) {
        // Current algorithm not in priority list, return first in list
        return m_fallbackPriority.empty() ? "" : m_fallbackPriority[0];
    }

    // Move to next algorithm
    ++it;
    if (it != m_fallbackPriority.end()) {
        return *it;
    }

    // No more algorithms in priority list
    // If classical fallback enabled, return classical algorithm
    if (m_enableClassicalFallback) {
        return "AES-256-GCM";  // Default classical fallback
    }

    return "";
}

