#ifndef INCIDENT_RESPONSE_WORKFLOW_H
#define INCIDENT_RESPONSE_WORKFLOW_H

#include "ml_threat_detector.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

/**
 * @brief Workflow Step
 */
struct WorkflowStep {
    QString id;
    QString name;
    QString action;
    QVariantMap parameters;
    bool completed;
};

/**
 * @brief Incident Response Workflow
 * 
 * Automated incident response workflows.
 */
class IncidentResponseWorkflow : public QObject {
    Q_OBJECT

public:
    explicit IncidentResponseWorkflow(QObject* parent = nullptr);
    ~IncidentResponseWorkflow();

    /**
     * @brief Initialize incident response workflow
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create workflow
     * @param name Workflow name
     * @param steps List of workflow steps
     * @return Workflow ID or empty if failed
     */
    QString createWorkflow(const QString& name, const QList<WorkflowStep>& steps);

    /**
     * @brief Execute workflow
     * @param workflowId Workflow ID
     * @param threat Threat that triggered workflow
     * @return true if execute successful, false otherwise
     */
    bool executeWorkflow(const QString& workflowId, const MLThreatResult& threat);

    /**
     * @brief Get workflow status
     * @param workflowId Workflow ID
     * @return Workflow status
     */
    QVariantMap getWorkflowStatus(const QString& workflowId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when workflow step completes
     */
    void stepCompleted(const QString& workflowId, const QString& stepId);

    /**
     * @brief Emitted when workflow completes
     */
    void workflowCompleted(const QString& workflowId, bool success);

private:
    QMap<QString, QVariantMap> m_workflows;
    
    bool executeStep(const WorkflowStep& step, const MLThreatResult& threat);
    QString generateWorkflowId() const;
};

Q_DECLARE_METATYPE(WorkflowStep)

#endif // INCIDENT_RESPONSE_WORKFLOW_H

