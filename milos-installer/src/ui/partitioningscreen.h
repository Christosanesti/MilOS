#ifndef PARTITIONINGSCREEN_H
#define PARTITIONINGSCREEN_H

#include <QObject>
#include <QQuickItem>

class PartitioningScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit PartitioningScreen(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
};

#endif // PARTITIONINGSCREEN_H

