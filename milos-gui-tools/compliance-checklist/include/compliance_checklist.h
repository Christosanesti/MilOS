#ifndef COMPLIANCE_CHECKLIST_H
#define COMPLIANCE_CHECKLIST_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>
#include <QList>

/**
 * @brief Compliance Standard
 */
enum class ComplianceStandard {
    NIST,
    CommonCriteria,
    STIG,
    Custom
};

/**
 * @brief Checklist Item Status
 */
enum class ChecklistItemStatus {
    NotStarted,
    InProgress,
    Completed,
    Failed,
    NotApplicable
};

/**
 * @brief Checklist Item
 */
struct ChecklistItem {
    QString id;
    QString title;
    QString description;
    ChecklistItemStatus status;
    QStringList evidence;
    QString notes;
    QDateTime completedAt;
    QString completedBy;
    QVariantMap metadata;
};

/**
 * @brief Compliance Checklist
 */
struct ComplianceChecklist {
    QString id;
    ComplianceStandard standard;
    QString name;
    QString description;
    QList<ChecklistItem> items;
    QDateTime createdAt;
    QDateTime lastUpdated;
    int completionPercentage;
};

/**
 * @brief Compliance Checklist Manager
 * 
 * Manages compliance checklists for security standards.
 */
class ComplianceChecklistManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList availableStandards READ availableStandards CONSTANT)

public:
    explicit ComplianceChecklistManager(QObject* parent = nullptr);
    ~ComplianceChecklistManager();

    /**
     * @brief Initialize checklist manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get available standards
     * @return List of available standards
     */
    QStringList availableStandards() const;

    /**
     * @brief Create checklist for standard
     * @param standard Compliance standard
     * @param name Checklist name
     * @return Checklist ID or empty if failed
     */
    QString createChecklist(ComplianceStandard standard, const QString& name = QString());

    /**
     * @brief Get checklist
     * @param checklistId Checklist ID
     * @return Checklist or empty if not found
     */
    ComplianceChecklist getChecklist(const QString& checklistId) const;

    /**
     * @brief Get all checklists
     * @return List of checklists
     */
    QList<ComplianceChecklist> getAllChecklists() const;

    /**
     * @brief Update checklist item status
     * @param checklistId Checklist ID
     * @param itemId Item ID
     * @param status New status
     * @param notes Optional notes
     * @return true if update successful, false otherwise
     */
    bool updateItemStatus(const QString& checklistId, const QString& itemId,
                         ChecklistItemStatus status, const QString& notes = QString());

    /**
     * @brief Add evidence to checklist item
     * @param checklistId Checklist ID
     * @param itemId Item ID
     * @param evidence Evidence file path or description
     * @return true if add successful, false otherwise
     */
    bool addEvidence(const QString& checklistId, const QString& itemId, const QString& evidence);

    /**
     * @brief Get checklist progress
     * @param checklistId Checklist ID
     * @return Progress percentage (0-100)
     */
    int getProgress(const QString& checklistId) const;

    /**
     * @brief Generate compliance report
     * @param checklistId Checklist ID
     * @return Report data
     */
    QVariantMap generateReport(const QString& checklistId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when checklist is created
     */
    void checklistCreated(const QString& checklistId);

    /**
     * @brief Emitted when item status is updated
     */
    void itemStatusUpdated(const QString& checklistId, const QString& itemId, ChecklistItemStatus status);

    /**
     * @brief Emitted when progress changes
     */
    void progressChanged(const QString& checklistId, int percentage);

private:
    QMap<QString, ComplianceChecklist> m_checklists;
    
    QString generateChecklistId() const;
    QList<ChecklistItem> loadStandardItems(ComplianceStandard standard) const;
    ChecklistItem createItem(const QString& id, const QString& title, const QString& description) const;
    void updateProgress(const QString& checklistId);
    QString standardToString(ComplianceStandard standard) const;
};

Q_DECLARE_METATYPE(ComplianceStandard)
Q_DECLARE_METATYPE(ChecklistItemStatus)
Q_DECLARE_METATYPE(ChecklistItem)
Q_DECLARE_METATYPE(ComplianceChecklist)

#endif // COMPLIANCE_CHECKLIST_H

