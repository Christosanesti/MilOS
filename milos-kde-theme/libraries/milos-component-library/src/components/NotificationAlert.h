#ifndef NOTIFICATIONALERT_H
#define NOTIFICATIONALERT_H

#include <QObject>
#include <QQuickItem>

class NotificationAlert : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(bool dismissible READ dismissible WRITE setDismissible NOTIFY dismissibleChanged)
    Q_PROPERTY(bool requiresAcknowledgment READ requiresAcknowledgment WRITE setRequiresAcknowledgment NOTIFY requiresAcknowledgmentChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)

public:
    explicit NotificationAlert(QQuickItem *parent = nullptr);
    
    QString variant() const { return m_variant; }
    void setVariant(const QString &variant);
    
    QString state() const { return m_state; }
    void setState(const QString &state);
    
    QString title() const { return m_title; }
    void setTitle(const QString &title);
    
    QString message() const { return m_message; }
    void setMessage(const QString &message);
    
    bool dismissible() const { return m_dismissible; }
    void setDismissible(bool dismissible);
    
    bool requiresAcknowledgment() const { return m_requiresAcknowledgment; }
    void setRequiresAcknowledgment(bool required);
    
    int duration() const { return m_duration; }
    void setDuration(int duration);

signals:
    void variantChanged();
    void stateChanged();
    void titleChanged();
    void messageChanged();
    void dismissibleChanged();
    void requiresAcknowledgmentChanged();
    void durationChanged();
    void dismissed();
    void acknowledged();

private:
    QString m_variant;
    QString m_state;
    QString m_title;
    QString m_message;
    bool m_dismissible;
    bool m_requiresAcknowledgment;
    int m_duration;
};

#endif // NOTIFICATIONALERT_H

