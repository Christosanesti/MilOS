#include "query_engine.h"
#include "config_parser.h"
#include "log_storage.h"
#include "log_storage.h"
#include <milos/logging/logger.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <sstream>

QueryEngine::QueryEngine()
    : m_initialized(false)
    , m_configParser(nullptr)
    , m_logStorage(nullptr)
{
}

QueryEngine::~QueryEngine() {
}

bool QueryEngine::initialize(ConfigParser* configParser, LogStorage* logStorage) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;
    m_logStorage = logStorage;

    m_initialized = true;
    return true;
}

std::string QueryEngine::queryLogs(const std::string& queryParams) {
    if (!m_initialized || !m_logStorage) {
        return "{\"error\":\"Query engine not initialized\"}";
    }

    // Parse query parameters
    std::map<std::string, std::string> filters = parseQueryParams(queryParams);

    // Get pagination parameters
    int limit = 100;
    int offset = 0;
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(queryParams).toUtf8(), &error);
    if (error.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("limit")) {
            limit = obj["limit"].toInt();
        }
        if (obj.contains("offset")) {
            offset = obj["offset"].toInt();
        }
    }

    // Query log entries
    std::vector<AuditLogEntry> entries = m_logStorage->queryLogEntries(filters, limit, offset);

    // Convert to JSON
    QJsonArray entriesArray;
    for (const auto& entry : entries) {
        QJsonObject entryObj;
        entryObj["log_id"] = QString::fromStdString(entry.log_id);
        entryObj["event_type"] = QString::fromStdString(entry.event_type);
        entryObj["event_severity"] = QString::fromStdString(entry.event_severity);
        entryObj["timestamp"] = QString::fromStdString(entry.timestamp);
        if (!entry.user_id.empty()) {
            entryObj["user_id"] = QString::fromStdString(entry.user_id);
        }
        entryObj["application_name"] = QString::fromStdString(entry.application_name);
        if (!entry.event_description.empty()) {
            entryObj["event_description"] = QString::fromStdString(entry.event_description);
        }
        entryObj["event_data"] = QString::fromStdString(entry.event_data);
        if (!entry.ip_address.empty()) {
            entryObj["ip_address"] = QString::fromStdString(entry.ip_address);
        }
        if (!entry.policy_id.empty()) {
            entryObj["policy_id"] = QString::fromStdString(entry.policy_id);
        }
        if (!entry.action_taken.empty()) {
            entryObj["action_taken"] = QString::fromStdString(entry.action_taken);
        }
        entryObj["log_hash"] = QString::fromStdString(entry.log_hash);
        if (!entry.previous_log_hash.empty()) {
            entryObj["previous_log_hash"] = QString::fromStdString(entry.previous_log_hash);
        }
        entriesArray.append(entryObj);
    }

    QJsonObject result;
    result["entries"] = entriesArray;
    result["count"] = static_cast<int>(entries.size());
    result["limit"] = limit;
    result["offset"] = offset;

    QJsonDocument resultDoc(result);
    return QString::fromUtf8(resultDoc.toJson()).toStdString();
}

std::string QueryEngine::getStatistics(const std::string& queryParams) {
    if (!m_initialized || !m_logStorage) {
        return "{\"error\":\"Query engine not initialized\"}";
    }

    // Parse query parameters
    std::map<std::string, std::string> filters = parseQueryParams(queryParams);

    // Get statistics
    std::map<std::string, int> stats = m_logStorage->getEventStatistics(filters);

    // Convert to JSON
    QJsonObject statsObj;
    for (const auto& stat : stats) {
        statsObj[QString::fromStdString(stat.first)] = stat.second;
    }

    QJsonDocument doc(statsObj);
    return QString::fromUtf8(doc.toJson()).toStdString();
}

std::string QueryEngine::exportLogs(const std::string& timeRange, const std::string& format) {
    if (!m_initialized || !m_logStorage) {
        return "";
    }

    // Parse time range
    std::map<std::string, std::string> filters = parseQueryParams(timeRange);

    // Query all entries in time range (no limit for export)
    std::vector<AuditLogEntry> entries = m_logStorage->queryLogEntries(filters, 100000, 0);

    // Export in requested format
    if (format == "json") {
        return exportToJSON(entries);
    } else if (format == "csv") {
        return exportToCSV(entries);
    } else if (format == "xml") {
        return exportToXML(entries);
    } else {
        LOG_ERROR(QString("Unsupported export format: %1").arg(QString::fromStdString(format)));
        return "";
    }
}

std::map<std::string, std::string> QueryEngine::parseQueryParams(const std::string& queryParams) {
    std::map<std::string, std::string> filters;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(queryParams).toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return filters;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("event_type")) {
        filters["event_type"] = obj["event_type"].toString().toStdString();
    }
    if (obj.contains("event_severity")) {
        filters["event_severity"] = obj["event_severity"].toString().toStdString();
    }
    if (obj.contains("user_id")) {
        filters["user_id"] = obj["user_id"].toString().toStdString();
    }
    if (obj.contains("timestamp_from")) {
        filters["timestamp_from"] = obj["timestamp_from"].toString().toStdString();
    }
    if (obj.contains("timestamp_to")) {
        filters["timestamp_to"] = obj["timestamp_to"].toString().toStdString();
    }

    return filters;
}

std::string QueryEngine::exportToJSON(const std::vector<AuditLogEntry>& entries) {
    QJsonArray entriesArray;
    for (const auto& entry : entries) {
        QJsonObject entryObj;
        entryObj["log_id"] = QString::fromStdString(entry.log_id);
        entryObj["event_type"] = QString::fromStdString(entry.event_type);
        entryObj["event_severity"] = QString::fromStdString(entry.event_severity);
        entryObj["timestamp"] = QString::fromStdString(entry.timestamp);
        entryObj["application_name"] = QString::fromStdString(entry.application_name);
        entryObj["event_data"] = QString::fromStdString(entry.event_data);
        entriesArray.append(entryObj);
    }

    QJsonObject result;
    result["entries"] = entriesArray;
    result["count"] = static_cast<int>(entries.size());

    QJsonDocument doc(result);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented)).toStdString();
}

std::string QueryEngine::exportToCSV(const std::vector<AuditLogEntry>& entries) {
    std::ostringstream csv;
    
    // CSV header
    csv << "log_id,event_type,event_severity,timestamp,application_name,event_description\n";
    
    // CSV rows
    for (const auto& entry : entries) {
        csv << entry.log_id << ",";
        csv << entry.event_type << ",";
        csv << entry.event_severity << ",";
        csv << entry.timestamp << ",";
        csv << entry.application_name << ",";
        csv << "\"" << entry.event_description << "\"\n";
    }
    
    return csv.str();
}

std::string QueryEngine::exportToXML(const std::vector<AuditLogEntry>& entries) {
    std::ostringstream xml;
    
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<audit_logs>\n";
    
    for (const auto& entry : entries) {
        xml << "  <entry>\n";
        xml << "    <log_id>" << entry.log_id << "</log_id>\n";
        xml << "    <event_type>" << entry.event_type << "</event_type>\n";
        xml << "    <event_severity>" << entry.event_severity << "</event_severity>\n";
        xml << "    <timestamp>" << entry.timestamp << "</timestamp>\n";
        xml << "    <application_name>" << entry.application_name << "</application_name>\n";
        xml << "    <event_description>" << entry.event_description << "</event_description>\n";
        xml << "  </entry>\n";
    }
    
    xml << "</audit_logs>\n";
    
    return xml.str();
}

