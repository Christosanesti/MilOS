#ifndef STATUSINDICATOR_H
#define STATUSINDICATOR_H

#include <QObject>
#include <QQuickItem>

class StatusIndicator : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)

public:
    explicit StatusIndicator(QQuickItem *parent = nullptr);
    
    QString variant() const { return m_variant; }
    void setVariant(const QString &variant);
    
    QString state() const { return m_state; }
    void setState(const QString &state);
    
    QString text() const { return m_text; }
    void setText(const QString &text);
    
    QString icon() const { return m_icon; }
    void setIcon(const QString &icon);

signals:
    void variantChanged();
    void stateChanged();
    void textChanged();
    void iconChanged();

private:
    QString m_variant;
    QString m_state;
    QString m_text;
    QString m_icon;
};

#endif // STATUSINDICATOR_H

