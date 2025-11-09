#include "DropdownMenu.h"

DropdownMenu::DropdownMenu(QQuickItem *parent)
    : QQuickItem(parent)
    , m_variant("standard")
    , m_open(false)
    , m_selectedValue("")
    , m_searchText("")
    , m_placeholder("Select...")
    , m_enabled(true)
{
    setFlag(ItemIsFocusScope, true);
}

void DropdownMenu::setVariant(const QString &variant)
{
    if (m_variant != variant) {
        m_variant = variant;
        emit variantChanged();
    }
}

void DropdownMenu::setOpen(bool open)
{
    if (m_open != open) {
        m_open = open;
        emit openChanged();
    }
}

void DropdownMenu::setItems(const QVariantList &items)
{
    if (m_items != items) {
        m_items = items;
        emit itemsChanged();
    }
}

void DropdownMenu::setSelectedItems(const QVariantList &items)
{
    if (m_selectedItems != items) {
        m_selectedItems = items;
        emit selectedItemsChanged();
    }
}

void DropdownMenu::setSelectedValue(const QString &value)
{
    if (m_selectedValue != value) {
        m_selectedValue = value;
        emit selectedValueChanged();
    }
}

void DropdownMenu::setSearchText(const QString &text)
{
    if (m_searchText != text) {
        m_searchText = text;
        emit searchTextChanged();
    }
}

void DropdownMenu::setPlaceholder(const QString &placeholder)
{
    if (m_placeholder != placeholder) {
        m_placeholder = placeholder;
        emit placeholderChanged();
    }
}

void DropdownMenu::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enabledChanged();
    }
}

