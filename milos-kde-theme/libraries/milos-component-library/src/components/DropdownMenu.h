#ifndef DROPDOWNMENU_H
#define DROPDOWNMENU_H

#include <QObject>
#include <QQuickItem>
#include <QVariantList>

class DropdownMenu : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(bool open READ open WRITE setOpen NOTIFY openChanged)
    Q_PROPERTY(QVariantList items READ items WRITE setItems NOTIFY itemsChanged)
    Q_PROPERTY(QVariantList selectedItems READ selectedItems WRITE setSelectedItems NOTIFY selectedItemsChanged)
    Q_PROPERTY(QString selectedValue READ selectedValue WRITE setSelectedValue NOTIFY selectedValueChanged)
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    Q_PROPERTY(QString placeholder READ placeholder WRITE setPlaceholder NOTIFY placeholderChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit DropdownMenu(QQuickItem *parent = nullptr);
    
    QString variant() const { return m_variant; }
    void setVariant(const QString &variant);
    
    bool open() const { return m_open; }
    void setOpen(bool open);
    
    QVariantList items() const { return m_items; }
    void setItems(const QVariantList &items);
    
    QVariantList selectedItems() const { return m_selectedItems; }
    void setSelectedItems(const QVariantList &items);
    
    QString selectedValue() const { return m_selectedValue; }
    void setSelectedValue(const QString &value);
    
    QString searchText() const { return m_searchText; }
    void setSearchText(const QString &text);
    
    QString placeholder() const { return m_placeholder; }
    void setPlaceholder(const QString &placeholder);
    
    bool enabled() const { return m_enabled; }
    void setEnabled(bool enabled);

signals:
    void variantChanged();
    void openChanged();
    void itemsChanged();
    void selectedItemsChanged();
    void selectedValueChanged();
    void searchTextChanged();
    void placeholderChanged();
    void enabledChanged();
    void itemSelected(const QString &value);

private:
    QString m_variant;
    bool m_open;
    QVariantList m_items;
    QVariantList m_selectedItems;
    QString m_selectedValue;
    QString m_searchText;
    QString m_placeholder;
    bool m_enabled;
};

#endif // DROPDOWNMENU_H

