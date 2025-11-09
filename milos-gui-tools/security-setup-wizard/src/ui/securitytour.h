#ifndef SECURITYTOUR_H
#define SECURITYTOUR_H

#include <QObject>
#include <QQuickItem>

class SecurityTour : public QQuickItem
{
    Q_OBJECT

public:
    explicit SecurityTour(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
};

#endif // SECURITYTOUR_H

