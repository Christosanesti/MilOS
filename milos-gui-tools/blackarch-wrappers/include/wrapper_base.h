#ifndef WRAPPER_BASE_H
#define WRAPPER_BASE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include "tool_executor.h"

/**
 * @brief Base class for GUI wrappers
 * 
 * Provides common functionality for GUI wrappers.
 */
class WrapperBase : public QObject {
    Q_OBJECT

public:
    explicit WrapperBase(const QString& toolName, QObject* parent = nullptr);
    ~WrapperBase();

    /**
     * @brief Get tool name
     */
    QString toolName() const { return m_toolName; }

    /**
     * @brief Execute tool with arguments
     * @param arguments Command arguments
     * @return true if execution started, false otherwise
     */
    Q_INVOKABLE bool execute(const QStringList& arguments = QStringList());

    /**
     * @brief Execute tool synchronously
     * @param arguments Command arguments
     * @param output Output string
     * @return true if execution successful, false otherwise
     */
    Q_INVOKABLE bool executeSync(const QStringList& arguments, QString* output = nullptr);

    /**
     * @brief Stop execution
     */
    Q_INVOKABLE void stop();

    /**
     * @brief Check if tool is running
     */
    Q_INVOKABLE bool isRunning() const;

Q_SIGNALS:
    /**
     * @brief Emitted when execution starts
     */
    void executionStarted();

    /**
     * @brief Emitted when execution completes
     */
    void executionCompleted(int exitCode);

    /**
     * @brief Emitted when output is available
     */
    void outputAvailable(const QString& output);

    /**
     * @brief Emitted when error output is available
     */
    void errorOutputAvailable(const QString& errorOutput);

protected:
    QString m_toolName;
    ToolExecutor* m_executor;
};

#endif // WRAPPER_BASE_H

