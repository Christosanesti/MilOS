#ifndef COMPATIBILITYDATABASE_H
#define COMPATIBILITYDATABASE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVariantList>

/**
 * @brief Theme Compatibility Database
 * 
 * Manages theme compatibility information and user reports
 */
class CompatibilityDatabase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList themes READ themes NOTIFY themesChanged)

public:
    enum CompatibilityStatus {
        Compatible,
        Incompatible,
        Unknown
    };
    Q_ENUM(CompatibilityStatus)

    explicit CompatibilityDatabase(QObject* parent = nullptr);
    ~CompatibilityDatabase();

    QVariantList themes() const { return m_themes; }

    Q_INVOKABLE CompatibilityStatus getThemeStatus(const QString& themeName);
    Q_INVOKABLE QVariantMap getThemeInfo(const QString& themeName);
    Q_INVOKABLE void reportThemeCompatibility(const QString& themeName, CompatibilityStatus status, const QString& details);
    Q_INVOKABLE void updateThemeStatus(const QString& themeName, CompatibilityStatus status, const QVariantMap& conflicts);
    Q_INVOKABLE QVariantList getCompatibleThemes();
    Q_INVOKABLE QVariantList getIncompatibleThemes();
    Q_INVOKABLE void loadDatabase();
    Q_INVOKABLE void saveDatabase();

signals:
    void themesChanged();
    void themeStatusUpdated(const QString& themeName, CompatibilityStatus status);

private:
    QString getDatabasePath() const;
    QString statusToString(CompatibilityStatus status);
    CompatibilityStatus stringToStatus(const QString& status);

    QVariantList m_themes;
};

#endif // COMPATIBILITYDATABASE_H

