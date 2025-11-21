#ifndef IMAGE_GENERATION_SERVICE_H
#define IMAGE_GENERATION_SERVICE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>
#include <QProcess>

class ImageGenerationService : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.ImageGeneration")

public:
    explicit ImageGenerationService(QObject *parent = nullptr);
    ~ImageGenerationService();

    // D-Bus methods
    Q_SCRIPTABLE QString generateImage(const QString &prompt, const QVariantMap &options = QVariantMap());
    Q_SCRIPTABLE QString editImage(const QString &imagePath, const QString &editPrompt, const QVariantMap &options = QVariantMap());
    Q_SCRIPTABLE QString restoreImage(const QString &imagePath, const QVariantMap &options = QVariantMap());
    
    Q_SCRIPTABLE bool isAvailable() const;
    Q_SCRIPTABLE QString getStatus() const;

signals:
    void imageGenerated(const QString &imagePath);
    void imageEdited(const QString &imagePath);
    void imageRestored(const QString &imagePath);
    void errorOccurred(const QString &error);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    QString callWrapper(const QString &command, const QStringList &args);
    QString findWrapperScript() const;
    QString m_wrapperPath;
    QProcess *m_process;
    QString m_outputDirectory;
    bool m_available;
};

#endif // IMAGE_GENERATION_SERVICE_H

