#ifndef PLASMAWIDGET_H
#define PLASMAWIDGET_H

#include <QObject>
#include <QQuickItem>

class PlasmaWidget : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString subtitle READ subtitle WRITE setSubtitle NOTIFY subtitleChanged)
    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)

public:
    explicit PlasmaWidget(QQuickItem *parent = nullptr);
    
    QString variant() const { return m_variant; }
    void setVariant(const QString &variant);
    
    QString state() const { return m_state; }
    void setState(const QString &state);
    
    QString status() const { return m_status; }
    void setStatus(const QString &status);
    
    QString title() const { return m_title; }
    void setTitle(const QString &title);
    
    QString subtitle() const { return m_subtitle; }
    void setSubtitle(const QString &subtitle);
    
    bool expanded() const { return m_expanded; }
    void setExpanded(bool expanded);

signals:
    void variantChanged();
    void stateChanged();
    void statusChanged();
    void titleChanged();
    void subtitleChanged();
    void expandedChanged();
    void actionTriggered(const QString &action);
    void emergencyActionTriggered();

private:
    QString m_variant;
    QString m_state;
    QString m_status;
    QString m_title;
    QString m_subtitle;
    bool m_expanded;
};

#endif // PLASMAWIDGET_H

