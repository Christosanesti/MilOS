#ifndef HARDENINGSERVICE_H
#define HARDENINGSERVICE_H

#include <QObject>
#include <QString>

class HardeningService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString level READ level NOTIFY levelChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)

public:
    explicit HardeningService(QObject *parent = nullptr);
    
    QString level() const { return m_level; }
    double progress() const { return m_progress; }
    
    Q_INVOKABLE void checkStatus();
    Q_INVOKABLE QString getLevel();

signals:
    void levelChanged();
    void progressChanged();

private:
    QString m_level; // basic, standard, high, maximum
    double m_progress; // 0.0 - 100.0
};

#endif // HARDENINGSERVICE_H

