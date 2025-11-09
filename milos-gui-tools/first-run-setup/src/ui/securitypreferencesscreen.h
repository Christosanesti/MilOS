#ifndef SECURITYPREFERENCESSCREEN_H
#define SECURITYPREFERENCESSCREEN_H

#include <QObject>
#include <QQuickItem>

class SecurityPreferencesScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit SecurityPreferencesScreen(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
};

#endif // SECURITYPREFERENCESSCREEN_H

