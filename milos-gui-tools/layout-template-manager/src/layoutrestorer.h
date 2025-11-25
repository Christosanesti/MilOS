#ifndef LAYOUTRESTORER_H
#define LAYOUTRESTORER_H

#include <QObject>
#include <QVariantMap>
#include <QString>

/**
 * @brief Layout Restorer
 * 
 * Saves and restores window layouts for template switching
 */
class LayoutRestorer : public QObject
{
    Q_OBJECT

public:
    explicit LayoutRestorer(QObject* parent = nullptr);

    /**
     * @brief Save current window layout
     * @return Saved layout data
     */
    QVariantMap saveCurrentLayout();

    /**
     * @brief Restore saved layout
     * @param layoutData Saved layout data
     * @return true if successful, false otherwise
     */
    bool restoreLayout(const QVariantMap& layoutData);

    /**
     * @brief Check if a saved layout exists
     * @return true if saved layout exists, false otherwise
     */
    bool hasSavedLayout() const;

signals:
    void layoutSaved(const QString& layoutId);
    void layoutRestored(bool success);
    void error(const QString& errorMessage);

private:
    QString getLayoutStoragePath() const;
    QString generateLayoutId() const;
    QVariantMap captureWindowPositions();
    bool applyWindowPositions(const QVariantMap& positions);

    QString m_savedLayoutId;

    // Make getLayoutStoragePath accessible to D-Bus interface
    friend class LayoutTemplateManagerDBus;
};

#endif // LAYOUTRESTORER_H

