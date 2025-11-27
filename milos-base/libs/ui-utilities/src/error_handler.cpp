#include "milos/ui/error_handler.h"
#include "milos/logging/logger.h"
#include <QStandardPaths>
#include <QDir>
#include <QMap>

ErrorHandler* ErrorHandler::s_instance = nullptr;

ErrorHandler::ErrorHandler(QObject* parent)
    : QObject(parent)
{
}

ErrorHandler::~ErrorHandler() {
}

ErrorHandler* ErrorHandler::instance() {
    if (!s_instance) {
        s_instance = new ErrorHandler();
    }
    return s_instance;
}

bool ErrorHandler::initialize(const QString& componentName) {
    m_componentName = componentName;
    
    if (!Logger::instance()->isInitialized()) {
        Logger::instance()->initialize(componentName + "-ErrorHandler");
    }
    
    return true;
}

void ErrorHandler::handleError(const QString& errorCode,
                               const QString& message,
                               ErrorSeverity severity,
                               ErrorCategory category,
                               const QVariantMap& context) {
    ErrorInfo errorInfo = createErrorInfo(errorCode, message, severity, category, context);
    handleError(errorInfo);
}

void ErrorHandler::handleError(const ErrorInfo& errorInfo) {
    m_lastError = errorInfo.message;
    m_lastErrorTitle = errorInfo.title;
    m_lastErrorInfo = errorInfo;
    
    // Log error
    QVariantMap logContext;
    logContext["error_code"] = errorInfo.errorCode;
    logContext["category"] = static_cast<int>(errorInfo.category);
    logContext["can_retry"] = errorInfo.canRetry;
    
    switch (errorInfo.severity) {
        case Critical:
            LOG_CRITICAL_CTX(QString("Critical error: %1").arg(errorInfo.message), logContext);
            break;
        case Error:
            LOG_ERROR_CTX(QString("Error: %1").arg(errorInfo.message), logContext);
            break;
        case Warning:
            LOG_WARNING_CTX(QString("Warning: %1").arg(errorInfo.message), logContext);
            break;
        case Info:
            LOG_INFO_CTX(QString("Info: %1").arg(errorInfo.message), logContext);
            break;
    }
    
    emit errorOccurred(errorInfo);
    emit errorStateChanged();
}

void ErrorHandler::clearError() {
    m_lastError.clear();
    m_lastErrorTitle.clear();
    m_lastErrorInfo = ErrorInfo();
    emit errorStateChanged();
}

QString ErrorHandler::getUserFriendlyMessage(const QString& errorCode) const {
    // Map technical error codes to user-friendly messages
    QMap<QString, QString> friendlyMessages = {
        {"NETWORK_CONNECTION_FAILED", "Unable to connect to the network. Please check your internet connection."},
        {"SERVICE_UNAVAILABLE", "The required service is currently unavailable. Please try again later."},
        {"AUTHENTICATION_FAILED", "Authentication failed. Please check your credentials and try again."},
        {"FILE_NOT_FOUND", "The requested file could not be found."},
        {"PERMISSION_DENIED", "You don't have permission to perform this action."},
        {"DISK_FULL", "The disk is full. Please free up space and try again."},
        {"INVALID_CONFIGURATION", "The configuration is invalid. Please check your settings."},
        {"HARDWARE_NOT_AVAILABLE", "Required hardware is not available or not properly configured."},
        {"OPERATION_TIMEOUT", "The operation took too long and timed out. Please try again."},
        {"UNKNOWN_ERROR", "An unexpected error occurred. Please try again or contact support."}
    };
    
    return friendlyMessages.value(errorCode, "An error occurred. Please try again.");
}

QStringList ErrorHandler::getErrorSuggestions(const QString& errorCode) const {
    return getSuggestionsForError(errorCode, m_lastErrorInfo.category);
}

bool ErrorHandler::canRetryError(const QString& errorCode) const {
    return isRetryableError(errorCode, m_lastErrorInfo.category);
}

void ErrorHandler::retryLastOperation() {
    if (!m_lastRetryAction.isEmpty()) {
        emit retryRequested(m_lastRetryAction);
    }
}

ErrorHandler::ErrorInfo ErrorHandler::createErrorInfo(const QString& errorCode,
                                                      const QString& message,
                                                      ErrorSeverity severity,
                                                      ErrorCategory category,
                                                      const QVariantMap& context) const {
    ErrorInfo info;
    info.errorCode = errorCode;
    info.message = message;
    info.detailedMessage = context.value("detailed_message", message).toString();
    info.severity = severity;
    info.category = category;
    info.title = getErrorTitle(severity, category);
    info.suggestions = getSuggestionsForError(errorCode, category);
    info.canRetry = isRetryableError(errorCode, category);
    info.retryAction = context.value("retry_action").toString();
    
    return info;
}

QString ErrorHandler::getErrorTitle(ErrorSeverity severity, ErrorCategory category) const {
    QString severityStr;
    switch (severity) {
        case Critical: severityStr = "Critical Error"; break;
        case Error: severityStr = "Error"; break;
        case Warning: severityStr = "Warning"; break;
        case Info: severityStr = "Information"; break;
    }
    
    QString categoryStr;
    switch (category) {
        case Network: categoryStr = "Network"; break;
        case FileSystem: categoryStr = "File System"; break;
        case Service: categoryStr = "Service"; break;
        case Authentication: categoryStr = "Authentication"; break;
        case Configuration: categoryStr = "Configuration"; break;
        case Hardware: categoryStr = "Hardware"; break;
        case Unknown: categoryStr = ""; break;
    }
    
    if (categoryStr.isEmpty()) {
        return severityStr;
    }
    return QString("%1 - %2").arg(severityStr, categoryStr);
}

QStringList ErrorHandler::getSuggestionsForError(const QString& errorCode, ErrorCategory category) const {
    QStringList suggestions;
    
    switch (category) {
        case Network:
            suggestions << "Check your internet connection"
                       << "Verify network settings"
                       << "Check firewall rules";
            break;
        case FileSystem:
            suggestions << "Check file permissions"
                       << "Verify disk space"
                       << "Check file path";
            break;
        case Service:
            suggestions << "Check if service is running"
                       << "Restart the service"
                       << "Check service logs";
            break;
        case Authentication:
            suggestions << "Verify your credentials"
                       << "Check account permissions"
                       << "Contact administrator";
            break;
        case Configuration:
            suggestions << "Check configuration file"
                       << "Verify settings"
                       << "Reset to defaults";
            break;
        case Hardware:
            suggestions << "Check hardware connections"
                       << "Verify drivers are installed"
                       << "Check hardware compatibility";
            break;
        default:
            suggestions << "Try again later"
                       << "Check system logs"
                       << "Contact support if problem persists";
    }
    
    return suggestions;
}

bool ErrorHandler::isRetryableError(const QString& errorCode, ErrorCategory category) const {
    // Network and service errors are generally retryable
    if (category == Network || category == Service) {
        return true;
    }
    
    // Specific retryable error codes
    QStringList retryableCodes = {
        "NETWORK_CONNECTION_FAILED",
        "SERVICE_UNAVAILABLE",
        "OPERATION_TIMEOUT",
        "TEMPORARY_ERROR"
    };
    
    return retryableCodes.contains(errorCode);
}


#include <QStandardPaths>
#include <QDir>
#include <QMap>

ErrorHandler* ErrorHandler::s_instance = nullptr;

ErrorHandler::ErrorHandler(QObject* parent)
    : QObject(parent)
{
}

ErrorHandler::~ErrorHandler() {
}

ErrorHandler* ErrorHandler::instance() {
    if (!s_instance) {
        s_instance = new ErrorHandler();
    }
    return s_instance;
}

bool ErrorHandler::initialize(const QString& componentName) {
    m_componentName = componentName;
    
    if (!Logger::instance()->isInitialized()) {
        Logger::instance()->initialize(componentName + "-ErrorHandler");
    }
    
    return true;
}

void ErrorHandler::handleError(const QString& errorCode,
                               const QString& message,
                               ErrorSeverity severity,
                               ErrorCategory category,
                               const QVariantMap& context) {
    ErrorInfo errorInfo = createErrorInfo(errorCode, message, severity, category, context);
    handleError(errorInfo);
}

void ErrorHandler::handleError(const ErrorInfo& errorInfo) {
    m_lastError = errorInfo.message;
    m_lastErrorTitle = errorInfo.title;
    m_lastErrorInfo = errorInfo;
    
    // Log error
    QVariantMap logContext;
    logContext["error_code"] = errorInfo.errorCode;
    logContext["category"] = static_cast<int>(errorInfo.category);
    logContext["can_retry"] = errorInfo.canRetry;
    
    switch (errorInfo.severity) {
        case Critical:
            LOG_CRITICAL_CTX(QString("Critical error: %1").arg(errorInfo.message), logContext);
            break;
        case Error:
            LOG_ERROR_CTX(QString("Error: %1").arg(errorInfo.message), logContext);
            break;
        case Warning:
            LOG_WARNING_CTX(QString("Warning: %1").arg(errorInfo.message), logContext);
            break;
        case Info:
            LOG_INFO_CTX(QString("Info: %1").arg(errorInfo.message), logContext);
            break;
    }
    
    emit errorOccurred(errorInfo);
    emit errorStateChanged();
}

void ErrorHandler::clearError() {
    m_lastError.clear();
    m_lastErrorTitle.clear();
    m_lastErrorInfo = ErrorInfo();
    emit errorStateChanged();
}

QString ErrorHandler::getUserFriendlyMessage(const QString& errorCode) const {
    // Map technical error codes to user-friendly messages
    QMap<QString, QString> friendlyMessages = {
        {"NETWORK_CONNECTION_FAILED", "Unable to connect to the network. Please check your internet connection."},
        {"SERVICE_UNAVAILABLE", "The required service is currently unavailable. Please try again later."},
        {"AUTHENTICATION_FAILED", "Authentication failed. Please check your credentials and try again."},
        {"FILE_NOT_FOUND", "The requested file could not be found."},
        {"PERMISSION_DENIED", "You don't have permission to perform this action."},
        {"DISK_FULL", "The disk is full. Please free up space and try again."},
        {"INVALID_CONFIGURATION", "The configuration is invalid. Please check your settings."},
        {"HARDWARE_NOT_AVAILABLE", "Required hardware is not available or not properly configured."},
        {"OPERATION_TIMEOUT", "The operation took too long and timed out. Please try again."},
        {"UNKNOWN_ERROR", "An unexpected error occurred. Please try again or contact support."}
    };
    
    return friendlyMessages.value(errorCode, "An error occurred. Please try again.");
}

QStringList ErrorHandler::getErrorSuggestions(const QString& errorCode) const {
    return getSuggestionsForError(errorCode, m_lastErrorInfo.category);
}

bool ErrorHandler::canRetryError(const QString& errorCode) const {
    return isRetryableError(errorCode, m_lastErrorInfo.category);
}

void ErrorHandler::retryLastOperation() {
    if (!m_lastRetryAction.isEmpty()) {
        emit retryRequested(m_lastRetryAction);
    }
}

ErrorHandler::ErrorInfo ErrorHandler::createErrorInfo(const QString& errorCode,
                                                      const QString& message,
                                                      ErrorSeverity severity,
                                                      ErrorCategory category,
                                                      const QVariantMap& context) const {
    ErrorInfo info;
    info.errorCode = errorCode;
    info.message = message;
    info.detailedMessage = context.value("detailed_message", message).toString();
    info.severity = severity;
    info.category = category;
    info.title = getErrorTitle(severity, category);
    info.suggestions = getSuggestionsForError(errorCode, category);
    info.canRetry = isRetryableError(errorCode, category);
    info.retryAction = context.value("retry_action").toString();
    
    return info;
}

QString ErrorHandler::getErrorTitle(ErrorSeverity severity, ErrorCategory category) const {
    QString severityStr;
    switch (severity) {
        case Critical: severityStr = "Critical Error"; break;
        case Error: severityStr = "Error"; break;
        case Warning: severityStr = "Warning"; break;
        case Info: severityStr = "Information"; break;
    }
    
    QString categoryStr;
    switch (category) {
        case Network: categoryStr = "Network"; break;
        case FileSystem: categoryStr = "File System"; break;
        case Service: categoryStr = "Service"; break;
        case Authentication: categoryStr = "Authentication"; break;
        case Configuration: categoryStr = "Configuration"; break;
        case Hardware: categoryStr = "Hardware"; break;
        case Unknown: categoryStr = ""; break;
    }
    
    if (categoryStr.isEmpty()) {
        return severityStr;
    }
    return QString("%1 - %2").arg(severityStr, categoryStr);
}

QStringList ErrorHandler::getSuggestionsForError(const QString& errorCode, ErrorCategory category) const {
    QStringList suggestions;
    
    switch (category) {
        case Network:
            suggestions << "Check your internet connection"
                       << "Verify network settings"
                       << "Check firewall rules";
            break;
        case FileSystem:
            suggestions << "Check file permissions"
                       << "Verify disk space"
                       << "Check file path";
            break;
        case Service:
            suggestions << "Check if service is running"
                       << "Restart the service"
                       << "Check service logs";
            break;
        case Authentication:
            suggestions << "Verify your credentials"
                       << "Check account permissions"
                       << "Contact administrator";
            break;
        case Configuration:
            suggestions << "Check configuration file"
                       << "Verify settings"
                       << "Reset to defaults";
            break;
        case Hardware:
            suggestions << "Check hardware connections"
                       << "Verify drivers are installed"
                       << "Check hardware compatibility";
            break;
        default:
            suggestions << "Try again later"
                       << "Check system logs"
                       << "Contact support if problem persists";
    }
    
    return suggestions;
}

bool ErrorHandler::isRetryableError(const QString& errorCode, ErrorCategory category) const {
    // Network and service errors are generally retryable
    if (category == Network || category == Service) {
        return true;
    }
    
    // Specific retryable error codes
    QStringList retryableCodes = {
        "NETWORK_CONNECTION_FAILED",
        "SERVICE_UNAVAILABLE",
        "OPERATION_TIMEOUT",
        "TEMPORARY_ERROR"
    };
    
    return retryableCodes.contains(errorCode);
}

