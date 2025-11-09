#ifndef SECURITYTOURSCREEN_H
#define SECURITYTOURSCREEN_H

#include <QObject>
#include <QQuickItem>

class SecurityTourScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit SecurityTourScreen(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
};

#endif // SECURITYTOURSCREEN_H

