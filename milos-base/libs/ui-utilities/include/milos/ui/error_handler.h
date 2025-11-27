#ifndef MILOS_UI_ERROR_HANDLER_H
#define MILOS_UI_ERROR_HANDLER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QStringList>

/**
 * @brief UI Error Handler
 * 
 * Provides centralized error handling and user-friendly error messages
 * for GUI applications with graceful degradation support.
 */
class ErrorHandler : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasError READ hasError NOTIFY errorStateChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorStateChanged)
    Q_PROPERTY(QString lastErrorTitle READ lastErrorTitle NOTIFY errorStateChanged)

public:
    /**
     * @brief Error severity levels
     */
    enum ErrorSeverity {
        Info = 0,       // Informational message
        Warning = 1,    // Warning - operation may have issues
        Error = 2,      // Error - operation failed
        Critical = 3    // Critical - system may be unstable
    };
    Q_ENUM(ErrorSeverity)

    /**
     * @brief Error categories
     */
    enum ErrorCategory {
        Network,
        FileSystem,
        Service,
        Authentication,
        Configuration,
        Hardware,
        Unknown
    };
    Q_ENUM(ErrorCategory)

    /**
     * @brief Error information structure
     */
    struct ErrorInfo {
        QString title;
        QString message;
        QString detailedMessage;
        ErrorSeverity severity;
        ErrorCategory category;
        QString errorCode;
        QStringList suggestions;
        bool canRetry;
        QString retryAction;
    };

    explicit ErrorHandler(QObject* parent = nullptr);
    ~ErrorHandler();

    /**
     * @brief Get singleton instance
     */
    static ErrorHandler* instance();

    /**
     * @brief Initialize error handler
     * @param componentName Component name for logging
     * @return true if successful
     */
    bool initialize(const QString& componentName = "UI");

    /**
     * @brief Handle error
     * @param errorCode Error code
     * @param message Error message
     * @param severity Error severity
     * @param category Error category
     * @param context Additional context
     */
    Q_INVOKABLE void handleError(const QString& errorCode,
                                  const QString& message,
                                  ErrorSeverity severity = Error,
                                  ErrorCategory category = Unknown,
                                  const QVariantMap& context = QVariantMap());

    /**
     * @brief Handle error with detailed information
     */
    void handleError(const ErrorInfo& errorInfo);

    /**
     * @brief Clear current error
     */
    Q_INVOKABLE void clearError();

    /**
     * @brief Get user-friendly error message
     */
    Q_INVOKABLE QString getUserFriendlyMessage(const QString& errorCode) const;

    /**
     * @brief Get error suggestions
     */
    Q_INVOKABLE QStringList getErrorSuggestions(const QString& errorCode) const;

    /**
     * @brief Check if error can be retried
     */
    Q_INVOKABLE bool canRetryError(const QString& errorCode) const;

    /**
     * @brief Retry last failed operation
     */
    Q_INVOKABLE void retryLastOperation();

    // Property getters
    bool hasError() const { return !m_lastError.isEmpty(); }
    QString lastError() const { return m_lastError; }
    QString lastErrorTitle() const { return m_lastErrorTitle; }

signals:
    /**
     * @brief Emitted when error occurs
     */
    void errorOccurred(const ErrorInfo& errorInfo);

    /**
     * @brief Emitted when error state changes
     */
    void errorStateChanged();

    /**
     * @brief Emitted when retry is requested
     */
    void retryRequested(const QString& action);

private:
    ErrorInfo createErrorInfo(const QString& errorCode,
                              const QString& message,
                              ErrorSeverity severity,
                              ErrorCategory category,
                              const QVariantMap& context) const;
    
    QString getErrorTitle(ErrorSeverity severity, ErrorCategory category) const;
    QStringList getSuggestionsForError(const QString& errorCode, ErrorCategory category) const;
    bool isRetryableError(const QString& errorCode, ErrorCategory category) const;

    static ErrorHandler* s_instance;
    QString m_componentName;
    QString m_lastError;
    QString m_lastErrorTitle;
    ErrorInfo m_lastErrorInfo;
    QString m_lastRetryAction;
};

#endif // MILOS_UI_ERROR_HANDLER_H



#define MILOS_UI_ERROR_HANDLER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QStringList>

/**
 * @brief UI Error Handler
 * 
 * Provides centralized error handling and user-friendly error messages
 * for GUI applications with graceful degradation support.
 */
class ErrorHandler : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasError READ hasError NOTIFY errorStateChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorStateChanged)
    Q_PROPERTY(QString lastErrorTitle READ lastErrorTitle NOTIFY errorStateChanged)

public:
    /**
     * @brief Error severity levels
     */
    enum ErrorSeverity {
        Info = 0,       // Informational message
        Warning = 1,    // Warning - operation may have issues
        Error = 2,      // Error - operation failed
        Critical = 3    // Critical - system may be unstable
    };
    Q_ENUM(ErrorSeverity)

    /**
     * @brief Error categories
     */
    enum ErrorCategory {
        Network,
        FileSystem,
        Service,
        Authentication,
        Configuration,
        Hardware,
        Unknown
    };
    Q_ENUM(ErrorCategory)

    /**
     * @brief Error information structure
     */
    struct ErrorInfo {
        QString title;
        QString message;
        QString detailedMessage;
        ErrorSeverity severity;
        ErrorCategory category;
        QString errorCode;
        QStringList suggestions;
        bool canRetry;
        QString retryAction;
    };

    explicit ErrorHandler(QObject* parent = nullptr);
    ~ErrorHandler();

    /**
     * @brief Get singleton instance
     */
    static ErrorHandler* instance();

    /**
     * @brief Initialize error handler
     * @param componentName Component name for logging
     * @return true if successful
     */
    bool initialize(const QString& componentName = "UI");

    /**
     * @brief Handle error
     * @param errorCode Error code
     * @param message Error message
     * @param severity Error severity
     * @param category Error category
     * @param context Additional context
     */
    Q_INVOKABLE void handleError(const QString& errorCode,
                                  const QString& message,
                                  ErrorSeverity severity = Error,
                                  ErrorCategory category = Unknown,
                                  const QVariantMap& context = QVariantMap());

    /**
     * @brief Handle error with detailed information
     */
    void handleError(const ErrorInfo& errorInfo);

    /**
     * @brief Clear current error
     */
    Q_INVOKABLE void clearError();

    /**
     * @brief Get user-friendly error message
     */
    Q_INVOKABLE QString getUserFriendlyMessage(const QString& errorCode) const;

    /**
     * @brief Get error suggestions
     */
    Q_INVOKABLE QStringList getErrorSuggestions(const QString& errorCode) const;

    /**
     * @brief Check if error can be retried
     */
    Q_INVOKABLE bool canRetryError(const QString& errorCode) const;

    /**
     * @brief Retry last failed operation
     */
    Q_INVOKABLE void retryLastOperation();

    // Property getters
    bool hasError() const { return !m_lastError.isEmpty(); }
    QString lastError() const { return m_lastError; }
    QString lastErrorTitle() const { return m_lastErrorTitle; }

signals:
    /**
     * @brief Emitted when error occurs
     */
    void errorOccurred(const ErrorInfo& errorInfo);

    /**
     * @brief Emitted when error state changes
     */
    void errorStateChanged();

    /**
     * @brief Emitted when retry is requested
     */
    void retryRequested(const QString& action);

private:
    ErrorInfo createErrorInfo(const QString& errorCode,
                              const QString& message,
                              ErrorSeverity severity,
                              ErrorCategory category,
                              const QVariantMap& context) const;
    
    QString getErrorTitle(ErrorSeverity severity, ErrorCategory category) const;
    QStringList getSuggestionsForError(const QString& errorCode, ErrorCategory category) const;
    bool isRetryableError(const QString& errorCode, ErrorCategory category) const;

    static ErrorHandler* s_instance;
    QString m_componentName;
    QString m_lastError;
    QString m_lastErrorTitle;
    ErrorInfo m_lastErrorInfo;
    QString m_lastRetryAction;
};

#endif // MILOS_UI_ERROR_HANDLER_H


