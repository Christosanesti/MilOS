#ifndef TABNAVIGATION_H
#define TABNAVIGATION_H

#include <QObject>
#include <QQuickItem>
#include <QVariantList>

class TabNavigation : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QVariantList tabs READ tabs WRITE setTabs NOTIFY tabsChanged)
    Q_PROPERTY(int maxTabs READ maxTabs WRITE setMaxTabs NOTIFY maxTabsChanged)

public:
    explicit TabNavigation(QQuickItem *parent = nullptr);
    
    QString variant() const { return m_variant; }
    void setVariant(const QString &variant);
    
    int currentIndex() const { return m_currentIndex; }
    void setCurrentIndex(int index);
    
    QVariantList tabs() const { return m_tabs; }
    void setTabs(const QVariantList &tabs);
    
    int maxTabs() const { return m_maxTabs; }
    void setMaxTabs(int maxTabs);

signals:
    void variantChanged();
    void currentIndexChanged();
    void tabsChanged();
    void maxTabsChanged();
    void tabChanged(int index);

private:
    QString m_variant;
    int m_currentIndex;
    QVariantList m_tabs;
    int m_maxTabs;
};

#endif // TABNAVIGATION_H

