#ifndef HARDENINGWIZARDSCREEN_H
#define HARDENINGWIZARDSCREEN_H

#include <QObject>
#include <QQuickItem>

class HardeningWizardScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit HardeningWizardScreen(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
};

#endif // HARDENINGWIZARDSCREEN_H

