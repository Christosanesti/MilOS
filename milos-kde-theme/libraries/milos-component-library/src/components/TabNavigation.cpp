#include "TabNavigation.h"

TabNavigation::TabNavigation(QQuickItem *parent)
    : QQuickItem(parent)
    , m_variant("standard")
    , m_currentIndex(0)
    , m_maxTabs(10)
{
    setFlag(ItemIsFocusScope, true);
}

void TabNavigation::setVariant(const QString &variant)
{
    if (m_variant != variant) {
        m_variant = variant;
        emit variantChanged();
    }
}

void TabNavigation::setCurrentIndex(int index)
{
    if (m_currentIndex != index && index >= 0 && index < m_tabs.size()) {
        m_currentIndex = index;
        emit currentIndexChanged();
        emit tabChanged(index);
    }
}

void TabNavigation::setTabs(const QVariantList &tabs)
{
    if (m_tabs != tabs) {
        m_tabs = tabs;
        emit tabsChanged();
        
        // Ensure currentIndex is valid
        if (m_currentIndex >= m_tabs.size()) {
            setCurrentIndex(0);
        }
    }
}

void TabNavigation::setMaxTabs(int maxTabs)
{
    if (m_maxTabs != maxTabs) {
        m_maxTabs = maxTabs;
        emit maxTabsChanged();
    }
}

