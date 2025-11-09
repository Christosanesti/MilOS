#ifndef SECURITYPREFERENCES_H
#define SECURITYPREFERENCES_H

#include <QObject>
#include <QQuickItem>

class SecurityPreferences : public QQuickItem
{
    Q_OBJECT

public:
    explicit SecurityPreferences(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
};

#endif // SECURITYPREFERENCES_H

