#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QQuickItem>

class MainWindow : public QQuickItem
{
    Q_OBJECT

public:
    explicit MainWindow(QQuickItem *parent = nullptr);
};

#endif // MAINWINDOW_H

