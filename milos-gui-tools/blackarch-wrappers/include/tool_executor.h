#ifndef TOOL_EXECUTOR_H
#define TOOL_EXECUTOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QByteArray>

/**
 * @brief Tool Executor
 * 
 * Executes command-line tools and captures output.
 */
class ToolExecutor : public QObject {
    Q_OBJECT

public:
    explicit ToolExecutor(QObject* parent = nullptr);
    ~ToolExecutor();

    /**
     * @brief Execute tool command
     * @param toolName Tool name
     * @param arguments Command arguments
     * @param workingDirectory Working directory (optional)
     * @return true if execution started successfully, false otherwise
     */
    bool execute(const QString& toolName, const QStringList& arguments = QStringList(), const QString& workingDirectory = QString());

    /**
     * @brief Execute tool command and wait for completion
     * @param toolName Tool name
     * @param arguments Command arguments
     * @param output Output string (stdout)
     * @param errorOutput Error output string (stderr)
     * @param exitCode Exit code
     * @return true if execution successful, false otherwise
     */
    bool executeSync(const QString& toolName, const QStringList& arguments = QStringList(),
                     QString* output = nullptr, QString* errorOutput = nullptr, int* exitCode = nullptr);

    /**
     * @brief Stop current execution
     */
    void stop();

    /**
     * @brief Check if tool is running
     * @return true if running, false otherwise
     */
    bool isRunning() const;

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

private slots:
    /**
     * @brief Handle process finished
     */
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    /**
     * @brief Handle process output
     */
    void onProcessOutput();

    /**
     * @brief Handle process error output
     */
    void onProcessErrorOutput();

private:
    QProcess* m_process;
    bool m_running;
};

#endif // TOOL_EXECUTOR_H

