#include "tool_executor.h"
#include <milos/logging/logger.h>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>

ToolExecutor::ToolExecutor(QObject* parent)
    : QObject(parent)
    , m_process(nullptr)
    , m_running(false)
{
}

ToolExecutor::~ToolExecutor() {
    stop();
    if (m_process) {
        delete m_process;
    }
}

bool ToolExecutor::execute(const QString& toolName, const QStringList& arguments, const QString& workingDirectory) {
    if (m_running) {
        LOG_WARNING("Tool execution already in progress");
        return false;
    }

    if (!m_process) {
        m_process = new QProcess(this);
        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &ToolExecutor::onProcessFinished);
        connect(m_process, &QProcess::readyReadStandardOutput,
                this, &ToolExecutor::onProcessOutput);
        connect(m_process, &QProcess::readyReadStandardError,
                this, &ToolExecutor::onProcessErrorOutput);
    }

    // Find tool executable
    QString executable = toolName;
    if (!QFileInfo(executable).isAbsolute()) {
        // Try which command
        QProcess whichProcess;
        whichProcess.start("which", QStringList() << toolName);
        whichProcess.waitForFinished();
        if (whichProcess.exitCode() == 0) {
            executable = QString::fromUtf8(whichProcess.readAllStandardOutput()).trimmed();
        }
    }

    m_process->setProgram(executable);
    m_process->setArguments(arguments);
    if (!workingDirectory.isEmpty()) {
        m_process->setWorkingDirectory(workingDirectory);
    }

    m_process->start();
    if (!m_process->waitForStarted()) {
        LOG_ERROR(QString("Failed to start tool: %1").arg(toolName));
        return false;
    }

    m_running = true;
    emit executionStarted();
    return true;
}

bool ToolExecutor::executeSync(const QString& toolName, const QStringList& arguments,
                                QString* output, QString* errorOutput, int* exitCode) {
    QProcess process;
    process.setProgram(toolName);
    process.setArguments(arguments);

    process.start();
    if (!process.waitForStarted()) {
        return false;
    }

    process.waitForFinished(-1);

    if (output) {
        *output = QString::fromUtf8(process.readAllStandardOutput());
    }
    if (errorOutput) {
        *errorOutput = QString::fromUtf8(process.readAllStandardError());
    }
    if (exitCode) {
        *exitCode = process.exitCode();
    }

    return process.exitCode() == 0;
}

void ToolExecutor::stop() {
    if (m_process && m_running) {
        m_process->kill();
        m_process->waitForFinished();
        m_running = false;
    }
}

bool ToolExecutor::isRunning() const {
    return m_running;
}

void ToolExecutor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    m_running = false;
    emit executionCompleted(exitCode);
}

void ToolExecutor::onProcessOutput() {
    if (m_process) {
        QByteArray data = m_process->readAllStandardOutput();
        QString output = QString::fromUtf8(data);
        emit outputAvailable(output);
    }
}

void ToolExecutor::onProcessErrorOutput() {
    if (m_process) {
        QByteArray data = m_process->readAllStandardError();
        QString errorOutput = QString::fromUtf8(data);
        emit errorOutputAvailable(errorOutput);
    }
}

