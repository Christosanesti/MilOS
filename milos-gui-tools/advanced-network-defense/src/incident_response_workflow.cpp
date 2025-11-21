#include "incident_response_workflow.h"
#include <QUuid>
#include <QDebug>

IncidentResponseWorkflow::IncidentResponseWorkflow(QObject* parent)
    : QObject(parent)
{
}

IncidentResponseWorkflow::~IncidentResponseWorkflow() {
}

bool IncidentResponseWorkflow::initialize() {
    return true;
}

QString IncidentResponseWorkflow::createWorkflow(const QString& name, const QList<WorkflowStep>& steps) {
    QString workflowId = generateWorkflowId();
    
    QVariantMap workflow;
    workflow["id"] = workflowId;
    workflow["name"] = name;
    workflow["status"] = "created";
    workflow["created_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QVariantList stepList;
    for (const WorkflowStep& step : steps) {
        QVariantMap stepMap;
        stepMap["id"] = step.id;
        stepMap["name"] = step.name;
        stepMap["action"] = step.action;
        stepMap["parameters"] = step.parameters;
        stepMap["completed"] = false;
        stepList.append(stepMap);
    }
    workflow["steps"] = stepList;
    
    m_workflows[workflowId] = workflow;
    
    return workflowId;
}

bool IncidentResponseWorkflow::executeWorkflow(const QString& workflowId, const MLThreatResult& threat) {
    if (!m_workflows.contains(workflowId)) {
        return false;
    }
    
    QVariantMap& workflow = m_workflows[workflowId];
    workflow["status"] = "executing";
    workflow["started_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QVariantList steps = workflow["steps"].toList();
    
    for (int i = 0; i < steps.size(); ++i) {
        QVariantMap stepMap = steps[i].toMap();
        WorkflowStep step;
        step.id = stepMap["id"].toString();
        step.name = stepMap["name"].toString();
        step.action = stepMap["action"].toString();
        step.parameters = stepMap["parameters"].toMap();
        step.completed = false;
        
        if (executeStep(step, threat)) {
            step.completed = true;
            stepMap["completed"] = true;
            steps[i] = stepMap;
            
            emit stepCompleted(workflowId, step.id);
        } else {
            workflow["status"] = "failed";
            workflow["failed_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
            emit workflowCompleted(workflowId, false);
            return false;
        }
    }
    
    workflow["steps"] = steps;
    workflow["status"] = "completed";
    workflow["completed_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    emit workflowCompleted(workflowId, true);
    
    return true;
}

QVariantMap IncidentResponseWorkflow::getWorkflowStatus(const QString& workflowId) const {
    return m_workflows.value(workflowId, QVariantMap());
}

bool IncidentResponseWorkflow::executeStep(const WorkflowStep& step, const MLThreatResult& threat) {
    // Execute workflow step based on action
    if (step.action == "contain") {
        // Contain threat
        qDebug() << "Containing threat:" << threat.id;
        return true;
    } else if (step.action == "block") {
        // Block network connection
        QString source = threat.metadata.value("source").toString();
        qDebug() << "Blocking connection from:" << source;
        return true;
    } else if (step.action == "quarantine") {
        // Quarantine resource
        qDebug() << "Quarantining resource";
        return true;
    } else if (step.action == "notify") {
        // Send notification
        qDebug() << "Sending notification";
        return true;
    } else if (step.action == "log") {
        // Log event
        qDebug() << "Logging event";
        return true;
    }
    
    return false;
}

QString IncidentResponseWorkflow::generateWorkflowId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

