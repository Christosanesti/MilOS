#ifndef CARD_H
#define CARD_H

#include <QObject>
#include <QQuickItem>

class Card : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString subtitle READ subtitle WRITE setSubtitle NOTIFY subtitleChanged)
    Q_PROPERTY(bool expandable READ expandable WRITE setExpandable NOTIFY expandableChanged)
    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(QColor statusColor READ statusColor WRITE setStatusColor NOTIFY statusColorChanged)

public:
    explicit Card(QQuickItem *parent = nullptr);
    
    QString variant() const { return m_variant; }
    void setVariant(const QString &variant);
    
    QString state() const { return m_state; }
    void setState(const QString &state);
    
    QString title() const { return m_title; }
    void setTitle(const QString &title);
    
    QString subtitle() const { return m_subtitle; }
    void setSubtitle(const QString &subtitle);
    
    bool expandable() const { return m_expandable; }
    void setExpandable(bool expandable);
    
    bool expanded() const { return m_expanded; }
    void setExpanded(bool expanded);
    
    QColor statusColor() const { return m_statusColor; }
    void setStatusColor(const QColor &color);

signals:
    void variantChanged();
    void stateChanged();
    void titleChanged();
    void subtitleChanged();
    void expandableChanged();
    void expandedChanged();
    void statusColorChanged();
    void clicked();

private:
    QString m_variant;
    QString m_state;
    QString m_title;
    QString m_subtitle;
    bool m_expandable;
    bool m_expanded;
    QColor m_statusColor;
};

#endif // CARD_H

