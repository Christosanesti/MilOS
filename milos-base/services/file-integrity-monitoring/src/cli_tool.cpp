#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusConnection>
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-fim");

    CLI::App cliApp{"MilOS File Integrity Monitoring CLI Tool", "milos-fim"};
    cliApp.require_subcommand(0, 1);

    // Create D-Bus interface
    QDBusInterface interface("org.milos.FileIntegrityMonitoring",
                             "/org/milos/FileIntegrityMonitoring",
                             "org.milos.FileIntegrityMonitoring",
                             QDBusConnection::systemBus());

    if (!interface.isValid()) {
        std::cerr << "Error: Cannot connect to File Integrity Monitoring service" << std::endl;
        std::cerr << "Make sure the service is running: systemctl start milos-file-integrity-monitoring" << std::endl;
        return 1;
    }

    // Baseline command
    auto* baselineCmd = cliApp.add_subcommand("baseline", "Manage baselines");
    std::string baselineAction;
    std::vector<std::string> baselineFiles;
    std::string baselineId;

    baselineCmd->add_option("action", baselineAction, "Action: create, update, status")
        ->required();
    baselineCmd->add_option("files", baselineFiles, "File paths");
    baselineCmd->add_option("--id", baselineId, "Baseline ID");

    // Verify command
    auto* verifyCmd = cliApp.add_subcommand("verify", "Verify file integrity");
    std::string verifyFile;
    verifyCmd->add_option("file", verifyFile, "File path to verify")->required();

    // Changes command
    auto* changesCmd = cliApp.add_subcommand("changes", "Show detected changes");
    std::string changesFilters;
    changesCmd->add_option("--filters", changesFilters, "Filter options (JSON)");

    // Status command
    auto* statusCmd = cliApp.add_subcommand("status", "Show FIM status");

    // Schedule command
    auto* scheduleCmd = cliApp.add_subcommand("schedule", "Manage verification schedules");
    std::string scheduleAction;
    std::string scheduleId;
    std::string scheduleJson;
    std::string scheduleName;
    std::string scheduleCron;
    std::vector<std::string> scheduleFiles;
    bool scheduleEnabled = true;
    scheduleCmd->add_option("action", scheduleAction, "Action: create, list, get, delete, enable, disable, history")
        ->required();
    scheduleCmd->add_option("--id", scheduleId, "Schedule ID");
    scheduleCmd->add_option("--json", scheduleJson, "Schedule JSON (for create)");
    scheduleCmd->add_option("--name", scheduleName, "Schedule name");
    scheduleCmd->add_option("--cron", scheduleCron, "Cron expression");
    scheduleCmd->add_option("--files", scheduleFiles, "File paths");
    scheduleCmd->add_flag("--enabled", scheduleEnabled, "Enable schedule");

    // Remediate command
    auto* remediateCmd = cliApp.add_subcommand("remediate", "Apply remediation for a detected change");
    std::string changeId;
    std::string requestId;
    std::string approver;
    std::string action;
    remediateCmd->add_option("change-id", changeId, "Change ID to remediate");
    remediateCmd->add_option("--request-id", requestId, "Remediation request ID");
    remediateCmd->add_option("--approve", approver, "Approve remediation request (provide approver name)");
    remediateCmd->add_option("--action", action, "Action: remediate, approve, status, pending");

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Handle baseline command
    if (baselineCmd->parsed()) {
        if (baselineAction == "create") {
            if (baselineFiles.empty()) {
                std::cerr << "Error: No files specified" << std::endl;
                return 1;
            }

            QStringList filePaths;
            for (const auto& file : baselineFiles) {
                filePaths << QString::fromStdString(file);
            }

            QDBusReply<QStringList> reply = interface.call("CreateBaseline", filePaths);
            if (reply.isValid()) {
                QStringList baselineIds = reply.value();
                std::cout << "Created " << baselineIds.size() << " baseline(s):" << std::endl;
                for (const QString& id : baselineIds) {
                    std::cout << "  " << id.toStdString() << std::endl;
                }
            } else {
                std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
                return 1;
            }
        } else if (baselineAction == "update") {
            if (baselineId.empty()) {
                std::cerr << "Error: Baseline ID required" << std::endl;
                return 1;
            }

            QDBusReply<bool> reply = interface.call("UpdateBaseline", QString::fromStdString(baselineId));
            if (reply.isValid() && reply.value()) {
                std::cout << "Baseline updated successfully" << std::endl;
            } else {
                std::cerr << "Error: Failed to update baseline" << std::endl;
                return 1;
            }
        } else if (baselineAction == "status") {
            if (baselineId.empty()) {
                std::cerr << "Error: Baseline ID required" << std::endl;
                return 1;
            }

            QDBusReply<QString> reply = interface.call("GetBaselineStatus", QString::fromStdString(baselineId));
            if (reply.isValid()) {
                std::cout << "Baseline status: " << reply.value().toStdString() << std::endl;
            } else {
                std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
                return 1;
            }
        }
    }

    // Handle verify command
    if (verifyCmd->parsed()) {
        QDBusReply<QString> reply = interface.call("VerifyIntegrity", QString::fromStdString(verifyFile));
        if (reply.isValid()) {
            QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                std::cout << "Verification result:" << std::endl;
                std::cout << "  File: " << obj["file_path"].toString().toStdString() << std::endl;
                std::cout << "  Valid: " << (obj["is_valid"].toBool() ? "yes" : "no") << std::endl;
                if (!obj["error_message"].toString().isEmpty()) {
                    std::cout << "  Error: " << obj["error_message"].toString().toStdString() << std::endl;
                }
            }
        } else {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
    }

    // Handle changes command
    if (changesCmd->parsed()) {
        QString filters = changesFilters.empty() ? "" : QString::fromStdString(changesFilters);
        QDBusReply<QString> reply = interface.call("GetChanges", filters);
        if (reply.isValid()) {
            QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
            if (doc.isArray()) {
                QJsonArray array = doc.array();
                std::cout << "Detected " << array.size() << " change(s):" << std::endl;
                for (const QJsonValue& value : array) {
                    QJsonObject obj = value.toObject();
                    std::cout << "  " << obj["file_path"].toString().toStdString()
                              << " [" << obj["severity"].toString().toStdString() << "]"
                              << " - " << obj["description"].toString().toStdString() << std::endl;
                }
            }
        } else {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
    }

    // Handle status command
    if (statusCmd->parsed()) {
        std::cout << "File Integrity Monitoring Service Status" << std::endl;
        std::cout << "Service: Running" << std::endl;
    }

    // Handle schedule command
    if (scheduleCmd->parsed()) {
        if (scheduleAction == "create") {
            // Create schedule
            QJsonObject scheduleObj;
            if (!scheduleJson.empty()) {
                // Use JSON if provided
                QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(scheduleJson).toUtf8());
                if (doc.isObject()) {
                    scheduleObj = doc.object();
                } else {
                    std::cerr << "Error: Invalid JSON" << std::endl;
                    return 1;
                }
            } else {
                // Build from command line options
                if (scheduleName.empty() || scheduleCron.empty()) {
                    std::cerr << "Error: --name and --cron required for create" << std::endl;
                    return 1;
                }
                scheduleObj["name"] = QString::fromStdString(scheduleName);
                scheduleObj["description"] = QString::fromStdString("CLI-created schedule");
                scheduleObj["cron_expression"] = QString::fromStdString(scheduleCron);
                scheduleObj["enabled"] = scheduleEnabled;
                QJsonArray filePathsArray;
                for (const auto& file : scheduleFiles) {
                    filePathsArray.append(QString::fromStdString(file));
                }
                scheduleObj["file_paths"] = filePathsArray;
            }

            QDBusReply<QString> reply = interface.call("CreateVerificationSchedule",
                                                        QJsonDocument(scheduleObj).toJson(QJsonDocument::Compact));
            if (reply.isValid()) {
                QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj["success"].toBool()) {
                        std::cout << "Schedule created successfully" << std::endl;
                        std::cout << "  Schedule ID: " << obj["schedule_id"].toString().toStdString() << std::endl;
                    } else {
                        std::cerr << "Error: " << obj["error"].toString().toStdString() << std::endl;
                        return 1;
                    }
                }
            } else {
                std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
                return 1;
            }
        } else if (scheduleAction == "list") {
            // List all schedules
            QDBusReply<QString> reply = interface.call("GetVerificationSchedules");
            if (reply.isValid()) {
                QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj["success"].toBool()) {
                        QJsonArray schedules = obj["schedules"].toArray();
                        std::cout << "Verification Schedules: " << schedules.size() << std::endl;
                        for (const QJsonValue& value : schedules) {
                            QJsonObject schedule = value.toObject();
                            std::cout << "  " << schedule["schedule_id"].toString().toStdString() << std::endl;
                            std::cout << "    Name: " << schedule["name"].toString().toStdString() << std::endl;
                            std::cout << "    Cron: " << schedule["cron_expression"].toString().toStdString() << std::endl;
                            std::cout << "    Enabled: " << (schedule["enabled"].toBool() ? "yes" : "no") << std::endl;
                            std::cout << "    Next Run: " << schedule["next_run_at"].toString().toStdString() << std::endl;
                            std::cout << std::endl;
                        }
                    } else {
                        std::cerr << "Error: " << obj["error"].toString().toStdString() << std::endl;
                        return 1;
                    }
                }
            } else {
                std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
                return 1;
            }
        } else if (scheduleAction == "get") {
            // Get schedule
            if (scheduleId.empty()) {
                std::cerr << "Error: --id required for get" << std::endl;
                return 1;
            }
            QDBusReply<QString> reply = interface.call("GetVerificationSchedule", QString::fromStdString(scheduleId));
            if (reply.isValid()) {
                QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj["success"].toBool()) {
                        std::cout << "Schedule Details:" << std::endl;
                        std::cout << "  ID: " << obj["schedule_id"].toString().toStdString() << std::endl;
                        std::cout << "  Name: " << obj["name"].toString().toStdString() << std::endl;
                        std::cout << "  Description: " << obj["description"].toString().toStdString() << std::endl;
                        std::cout << "  Cron: " << obj["cron_expression"].toString().toStdString() << std::endl;
                        std::cout << "  Enabled: " << (obj["enabled"].toBool() ? "yes" : "no") << std::endl;
                        std::cout << "  Created: " << obj["created_at"].toString().toStdString() << std::endl;
                        std::cout << "  Last Run: " << obj["last_run_at"].toString().toStdString() << std::endl;
                        std::cout << "  Next Run: " << obj["next_run_at"].toString().toStdString() << std::endl;
                    } else {
                        std::cerr << "Error: " << obj["error"].toString().toStdString() << std::endl;
                        return 1;
                    }
                }
            } else {
                std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
                return 1;
            }
        } else if (scheduleAction == "delete") {
            // Delete schedule
            if (scheduleId.empty()) {
                std::cerr << "Error: --id required for delete" << std::endl;
                return 1;
            }
            QDBusReply<bool> reply = interface.call("DeleteVerificationSchedule", QString::fromStdString(scheduleId));
            if (reply.isValid() && reply.value()) {
                std::cout << "Schedule deleted successfully" << std::endl;
            } else {
                std::cerr << "Error: Failed to delete schedule" << std::endl;
                return 1;
            }
        } else if (scheduleAction == "enable" || scheduleAction == "disable") {
            // Enable/disable schedule
            if (scheduleId.empty()) {
                std::cerr << "Error: --id required for enable/disable" << std::endl;
                return 1;
            }
            bool enabled = (scheduleAction == "enable");
            QDBusReply<bool> reply = interface.call("SetVerificationScheduleEnabled",
                                                     QString::fromStdString(scheduleId), enabled);
            if (reply.isValid() && reply.value()) {
                std::cout << "Schedule " << (enabled ? "enabled" : "disabled") << " successfully" << std::endl;
            } else {
                std::cerr << "Error: Failed to " << (enabled ? "enable" : "disable") << " schedule" << std::endl;
                return 1;
            }
        } else if (scheduleAction == "history") {
            // Get verification history
            QDBusReply<QString> reply = interface.call("GetVerificationHistory",
                                                       scheduleId.empty() ? "" : QString::fromStdString(scheduleId),
                                                       "",
                                                       100);
            if (reply.isValid()) {
                QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj["success"].toBool()) {
                        QJsonArray history = obj["history"].toArray();
                        std::cout << "Verification History: " << history.size() << " entries" << std::endl;
                        for (const QJsonValue& value : history) {
                            QJsonObject entry = value.toObject();
                            std::cout << "  " << entry["file_path"].toString().toStdString()
                                      << " [" << (entry["is_valid"].toBool() ? "VALID" : "INVALID") << "]"
                                      << " - " << entry["verified_at"].toString().toStdString() << std::endl;
                        }
                    } else {
                        std::cerr << "Error: " << obj["error"].toString().toStdString() << std::endl;
                        return 1;
                    }
                }
            } else {
                std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown action: " << scheduleAction << std::endl;
            return 1;
        }
    }

    // Handle remediate command
    if (remediateCmd->parsed()) {
        if (action.empty() && changeId.empty() && requestId.empty()) {
            std::cerr << "Error: Must specify either change-id, request-id with action, or use --action pending" << std::endl;
            return 1;
        }

        if (action == "pending") {
            // Get all pending remediations
            QDBusReply<QString> reply = interface.call("GetPendingRemediations");
            if (reply.isValid()) {
                QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj["success"].toBool()) {
                        QJsonArray requests = obj["requests"].toArray();
                        std::cout << "Pending remediation requests: " << requests.size() << std::endl;
                        for (const QJsonValue& value : requests) {
                            QJsonObject req = value.toObject();
                            std::cout << "  Request ID: " << req["request_id"].toString().toStdString() << std::endl;
                            std::cout << "    Change ID: " << req["change_id"].toString().toStdString() << std::endl;
                            std::cout << "    File: " << req["file_path"].toString().toStdString() << std::endl;
                            std::cout << "    Status: " << req["status"].toString().toStdString() << std::endl;
                            std::cout << "    Requested: " << req["requested_at"].toString().toStdString() << std::endl;
                            std::cout << std::endl;
                        }
                    } else {
                        std::cerr << "Error: " << obj["error"].toString().toStdString() << std::endl;
                        return 1;
                    }
                }
            } else {
                std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
                return 1;
            }
        } else if (action == "approve") {
            // Approve remediation request
            if (requestId.empty() || approver.empty()) {
                std::cerr << "Error: --request-id and --approve required for approve action" << std::endl;
                return 1;
            }

            QDBusReply<bool> reply = interface.call("ApproveRemediation",
                                                     QString::fromStdString(requestId),
                                                     QString::fromStdString(approver));
            if (reply.isValid() && reply.value()) {
                std::cout << "Remediation request approved successfully" << std::endl;
            } else {
                std::cerr << "Error: Failed to approve remediation request" << std::endl;
                return 1;
            }
        } else if (action == "status") {
            // Get remediation request status
            if (requestId.empty()) {
                std::cerr << "Error: --request-id required for status action" << std::endl;
                return 1;
            }

            QDBusReply<QString> reply = interface.call("GetRemediationStatus", QString::fromStdString(requestId));
            if (reply.isValid()) {
                QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj["success"].toBool()) {
                        std::cout << "Remediation Request Status:" << std::endl;
                        std::cout << "  Request ID: " << obj["request_id"].toString().toStdString() << std::endl;
                        std::cout << "  Change ID: " << obj["change_id"].toString().toStdString() << std::endl;
                        std::cout << "  File: " << obj["file_path"].toString().toStdString() << std::endl;
                        std::cout << "  Status: " << obj["status"].toString().toStdString() << std::endl;
                        std::cout << "  Requested: " << obj["requested_at"].toString().toStdString() << std::endl;
                        if (!obj["approved_at"].toString().isEmpty()) {
                            std::cout << "  Approved: " << obj["approved_at"].toString().toStdString() << std::endl;
                            std::cout << "  Approver: " << obj["approver"].toString().toStdString() << std::endl;
                        }
                        if (!obj["completed_at"].toString().isEmpty()) {
                            std::cout << "  Completed: " << obj["completed_at"].toString().toStdString() << std::endl;
                        }
                        if (!obj["error_message"].toString().isEmpty()) {
                            std::cout << "  Error: " << obj["error_message"].toString().toStdString() << std::endl;
                        }
                    } else {
                        std::cerr << "Error: " << obj["error"].toString().toStdString() << std::endl;
                        return 1;
                    }
                }
            } else {
                std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
                return 1;
            }
        } else if (!changeId.empty()) {
            // Remediate a change
            QDBusReply<QString> reply = interface.call("RemediateChange", QString::fromStdString(changeId));
            if (reply.isValid()) {
                QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj["success"].toBool()) {
                        std::cout << "Remediation initiated successfully" << std::endl;
                        std::cout << "  Request ID: " << obj["request_id"].toString().toStdString() << std::endl;
                        std::cout << "  Status: " << (obj.contains("status") ? obj["status"].toString().toStdString() : "executed") << std::endl;
                        std::cout << "  Message: " << obj["message"].toString().toStdString() << std::endl;
                        if (obj.contains("executed_at")) {
                            std::cout << "  Executed: " << obj["executed_at"].toString().toStdString() << std::endl;
                        }
                    } else {
                        std::cerr << "Error: " << obj["error"].toString().toStdString() << std::endl;
                        return 1;
                    }
                }
            } else {
                std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error: Invalid action or missing parameters" << std::endl;
            return 1;
        }
    }

    if (!cliApp.get_subcommands().empty() && !cliApp.get_subcommands()[0]->parsed()) {
        std::cout << cliApp.help() << std::endl;
    }

    return 0;
}

