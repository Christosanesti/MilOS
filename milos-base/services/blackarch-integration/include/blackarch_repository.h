#ifndef BLACKARCH_REPOSITORY_H
#define BLACKARCH_REPOSITORY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include "blackarch_data_scraper.h"

/**
 * @brief BlackArch Repository Manager
 * 
 * Manages BlackArch repository data access (scraped data, not installation).
 * This class provides access to scraped tool information from BlackArch repositories.
 */
class BlackArchRepository : public QObject {
    Q_OBJECT

public:
    explicit BlackArchRepository(QObject* parent = nullptr);
    ~BlackArchRepository();

    /**
     * @brief Initialize repository manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Check if BlackArch repository is configured
     * @return true if configured, false otherwise
     */
    bool isRepositoryConfigured() const;

    /**
     * @brief Configure BlackArch repository
     * @return true if configuration successful, false otherwise
     */
    bool configureRepository();

    /**
     * @brief Get list of available tools (from scraped data)
     * @return List of available tool names
     */
    QStringList getAvailableTools() const;

    /**
     * @brief Get tool information
     * @param toolName Tool name
     * @return Tool information as JSON string
     */
    QString getToolInfo(const QString& toolName) const;

    /**
     * @brief Get tools by category
     * @param category Category name
     * @return List of tool names in category
     */
    QStringList getToolsByCategory(const QString& category) const;

    /**
     * @brief Get all categories
     * @return List of category names
     */
    QStringList getCategories() const;

    /**
     * @brief Search tools
     * @param query Search query
     * @return List of matching tool names
     */
    QStringList searchTools(const QString& query) const;

    /**
     * @brief Refresh scraped data
     * @return true if refresh successful, false otherwise
     */
    bool refreshData();

Q_SIGNALS:
    /**
     * @brief Emitted when data scraping starts
     */
    void dataScrapingStarted();

    /**
     * @brief Emitted when data scraping completes
     */
    void dataScrapingCompleted(bool success);

    /**
     * @brief Emitted when data scraping progress changes
     */
    void dataScrapingProgress(int progress, const QString& currentTool);

private:
    bool m_initialized;
    QString m_dataStoragePath;
    BlackArchDataScraper* m_dataScraper;
};

#endif // BLACKARCH_REPOSITORY_H

