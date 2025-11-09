#ifndef PREFERENCESMANAGER_H
#define PREFERENCESMANAGER_H

#include <QObject>
#include <QVariantMap>

class PreferencesManager : public QObject
{
    Q_OBJECT

public:
    explicit PreferencesManager(QObject *parent = nullptr);
    
    Q_INVOKABLE void savePreferences(const QVariantMap &preferences);
    Q_INVOKABLE QVariantMap loadPreferences();
    Q_INVOKABLE void resetPreferences();

signals:
    void preferencesSaved();
    void preferencesLoaded(const QVariantMap &preferences);

private:
    QString m_preferencesFilePath;
};

#endif // PREFERENCESMANAGER_H

