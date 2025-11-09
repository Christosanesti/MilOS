#ifndef PACKAGEINSTALLATIONSCREEN_H
#define PACKAGEINSTALLATIONSCREEN_H

#include <QObject>
#include <QQuickItem>

class PackageInstallationScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit PackageInstallationScreen(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
    void errorOccurred(const QString &error);
};

#endif // PACKAGEINSTALLATIONSCREEN_H

