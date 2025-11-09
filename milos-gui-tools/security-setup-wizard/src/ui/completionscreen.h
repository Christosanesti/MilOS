#ifndef COMPLETIONSCREEN_H
#define COMPLETIONSCREEN_H

#include <QObject>
#include <QQuickItem>

class CompletionScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit CompletionScreen(QQuickItem *parent = nullptr);

signals:
    void finishClicked();
};

#endif // COMPLETIONSCREEN_H

