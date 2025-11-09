#include "mainwindow.h"

MainWindow::MainWindow(QQuickItem *parent)
    : QQuickItem(parent)
    , m_currentStep(0)
{
}

void MainWindow::setCurrentStep(int step)
{
    if (m_currentStep != step) {
        m_currentStep = step;
        emit currentStepChanged();
    }
}

