#ifndef HARDENINGWIZARD_H
#define HARDENINGWIZARD_H

#include <QObject>
#include <QQuickItem>

class HardeningWizard : public QQuickItem
{
    Q_OBJECT

public:
    explicit HardeningWizard(QQuickItem *parent = nullptr);

signals:
    void nextClicked();
};

#endif // HARDENINGWIZARD_H

