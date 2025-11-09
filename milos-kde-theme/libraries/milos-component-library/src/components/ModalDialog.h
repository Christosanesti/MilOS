#ifndef MODALDIALOG_H
#define MODALDIALOG_H

#include <QObject>
#include <QQuickItem>

class ModalDialog : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(bool critical READ critical WRITE setCritical NOTIFY criticalChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

public:
    explicit ModalDialog(QQuickItem *parent = nullptr);
    
    QString variant() const { return m_variant; }
    void setVariant(const QString &variant);
    
    QString state() const { return m_state; }
    void setState(const QString &state);
    
    QString title() const { return m_title; }
    void setTitle(const QString &title);
    
    QString message() const { return m_message; }
    void setMessage(const QString &message);
    
    bool critical() const { return m_critical; }
    void setCritical(bool critical);
    
    bool visible() const { return m_visible; }
    void setVisible(bool visible);

signals:
    void variantChanged();
    void stateChanged();
    void titleChanged();
    void messageChanged();
    void criticalChanged();
    void visibleChanged();
    void confirmed();
    void cancelled();

private:
    QString m_variant;
    QString m_state;
    QString m_title;
    QString m_message;
    bool m_critical;
    bool m_visible;
};

#endif // MODALDIALOG_H

