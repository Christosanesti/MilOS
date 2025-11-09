#ifndef SECURITYOVERVIEW_H
#define SECURITYOVERVIEW_H

#include <QObject>
#include <QQuickItem>

class SecurityOverview : public QQuickItem
{
    Q_OBJECT

public:
    explicit SecurityOverview(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
};

#endif // SECURITYOVERVIEW_H

