#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QQuickItem>

class MainWindow : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int currentStep READ currentStep WRITE setCurrentStep NOTIFY currentStepChanged)
    Q_PROPERTY(int totalSteps READ totalSteps WRITE setTotalSteps NOTIFY totalStepsChanged)

public:
    explicit MainWindow(QQuickItem *parent = nullptr);
    
    int currentStep() const { return m_currentStep; }
    void setCurrentStep(int step);
    
    int totalSteps() const { return m_totalSteps; }
    void setTotalSteps(int steps);

signals:
    void currentStepChanged();
    void totalStepsChanged();

private:
    int m_currentStep;
    int m_totalSteps;
};

#endif // MAINWINDOW_H

