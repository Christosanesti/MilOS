#include "wrapper_base.h"
#include "tool_executor.h"
#include <QDebug>

WrapperBase::WrapperBase(const QString& toolName, QObject* parent)
    : QObject(parent)
    , m_toolName(toolName)
    , m_executor(nullptr)
{
    m_executor = new ToolExecutor(this);
    connect(m_executor, &ToolExecutor::executionStarted,
            this, &WrapperBase::executionStarted);
    connect(m_executor, &ToolExecutor::executionCompleted,
            this, &WrapperBase::executionCompleted);
    connect(m_executor, &ToolExecutor::outputAvailable,
            this, &WrapperBase::outputAvailable);
    connect(m_executor, &ToolExecutor::errorOutputAvailable,
            this, &WrapperBase::errorOutputAvailable);
}

WrapperBase::~WrapperBase() {
}

bool WrapperBase::execute(const QStringList& arguments) {
    return m_executor->execute(m_toolName, arguments);
}

bool WrapperBase::executeSync(const QStringList& arguments, QString* output) {
    QString errorOutput;
    int exitCode;
    return m_executor->executeSync(m_toolName, arguments, output, &errorOutput, &exitCode);
}

void WrapperBase::stop() {
    m_executor->stop();
}

bool WrapperBase::isRunning() const {
    return m_executor->isRunning();
}

