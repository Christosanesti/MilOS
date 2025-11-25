#ifndef THEMECONFLICTDETECTOR_H
#define THEMECONFLICTDETECTOR_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QStringList>
#include "themevalidator.h"
#include "compatibilitydatabase.h"
#include "conflictdetectionrules.h"
#include "conflictnotifier.h"

/**
 * @brief Theme Conflict Detector Service
 * 
 * Monitors KDE Plasma theme changes and detects conflicts with MilOS xenon effects
 */
class ThemeConflictDetector : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.ThemeConflictDetector")

public:
    explicit ThemeConflictDetector(QObject* parent = nullptr);
    ~ThemeConflictDetector();

    bool initialize();
    bool start();
    void stop();
    bool isRunning() const { return m_running; }

public Q_SLOTS:
    // D-Bus methods
    QVariantMap ValidateTheme(const QString& themeId);
    QVariantList GetThemeConflicts(const QString& themeId);
    QString GetCompatibilityStatus(const QString& themeId);
    bool UpdateCompatibilityDatabase(const QString& themeId, const QString& status);

Q_SIGNALS:
    void ThemeConflictDetected(const QString& themeId, const QVariantList& conflicts);
    void ThemeValidated(const QString& themeId, const QString& status);

private Q_SLOTS:
    void onThemeChanged();
    void onThemeInstalled(const QString& themeId);

private:
    bool registerInterface();
    void unregisterInterface();
    void monitorThemeChanges();
    void validateCurrentTheme();

    ThemeValidator* m_validator;
    CompatibilityDatabase* m_compatibilityDatabase;
    ConflictDetectionRules* m_detectionRules;
    ConflictNotifier* m_notifier;
    bool m_running;
    bool m_initialized;
    QString m_currentThemeId;
};

#endif // THEMECONFLICTDETECTOR_H

