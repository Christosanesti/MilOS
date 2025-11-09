#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QQuickItem>

class MainWindow : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int currentStep READ currentStep WRITE setCurrentStep NOTIFY currentStepChanged)

public:
    explicit MainWindow(QQuickItem *parent = nullptr);
    
    int currentStep() const { return m_currentStep; }
    void setCurrentStep(int step);

signals:
    void currentStepChanged();

private:
    int m_currentStep;
};

#endif // MAINWINDOW_H

