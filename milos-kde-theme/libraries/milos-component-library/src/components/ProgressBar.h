#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QObject>
#include <QQuickItem>

class ProgressBar : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(qreal percentage READ percentage NOTIFY valueChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString details READ details WRITE setDetails NOTIFY detailsChanged)
    Q_PROPERTY(int stepCount READ stepCount WRITE setStepCount NOTIFY stepCountChanged)
    Q_PROPERTY(int currentStep READ currentStep WRITE setCurrentStep NOTIFY currentStepChanged)
    Q_PROPERTY(QString estimatedTime READ estimatedTime WRITE setEstimatedTime NOTIFY estimatedTimeChanged)

public:
    explicit ProgressBar(QQuickItem *parent = nullptr);
    
    QString variant() const { return m_variant; }
    void setVariant(const QString &variant);
    
    QString state() const { return m_state; }
    void setState(const QString &state);
    
    qreal value() const { return m_value; }
    void setValue(qreal value);
    
    qreal percentage() const { return m_value * 100.0; }
    
    QString text() const { return m_text; }
    void setText(const QString &text);
    
    QString details() const { return m_details; }
    void setDetails(const QString &details);
    
    int stepCount() const { return m_stepCount; }
    void setStepCount(int count);
    
    int currentStep() const { return m_currentStep; }
    void setCurrentStep(int step);
    
    QString estimatedTime() const { return m_estimatedTime; }
    void setEstimatedTime(const QString &time);

signals:
    void variantChanged();
    void stateChanged();
    void valueChanged();
    void textChanged();
    void detailsChanged();
    void stepCountChanged();
    void currentStepChanged();
    void estimatedTimeChanged();

private:
    QString m_variant;
    QString m_state;
    qreal m_value;
    QString m_text;
    QString m_details;
    int m_stepCount;
    int m_currentStep;
    QString m_estimatedTime;
};

#endif // PROGRESSBAR_H

