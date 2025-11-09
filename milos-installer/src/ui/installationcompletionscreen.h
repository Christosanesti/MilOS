#ifndef INSTALLATIONCOMPLETIONSCREEN_H
#define INSTALLATIONCOMPLETIONSCREEN_H

#include <QObject>
#include <QQuickItem>

class InstallationCompletionScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit InstallationCompletionScreen(QQuickItem *parent = nullptr);

signals:
    void rebootClicked();
};

#endif // INSTALLATIONCOMPLETIONSCREEN_H

