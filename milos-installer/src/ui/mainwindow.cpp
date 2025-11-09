#include "mainwindow.h"

MainWindow::MainWindow(QQuickItem *parent)
    : QQuickItem(parent)
    , m_currentStep(0)
    , m_totalSteps(2)
{
}

void MainWindow::setCurrentStep(int step)
{
    if (m_currentStep != step) {
        m_currentStep = step;
        emit currentStepChanged();
    }
}

void MainWindow::setTotalSteps(int steps)
{
    if (m_totalSteps != steps) {
        m_totalSteps = steps;
        emit totalStepsChanged();
    }
}

