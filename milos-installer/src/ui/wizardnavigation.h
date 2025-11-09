#ifndef WIZARDNAVIGATION_H
#define WIZARDNAVIGATION_H

#include <QObject>
#include <QQuickItem>

class WizardNavigation : public QQuickItem
{
    Q_OBJECT

public:
    explicit WizardNavigation(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
};

#endif // WIZARDNAVIGATION_H

