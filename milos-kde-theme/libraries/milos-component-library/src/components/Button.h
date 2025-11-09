#ifndef BUTTON_H
#define BUTTON_H

#include <QObject>
#include <QQuickItem>

class Button : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit Button(QQuickItem *parent = nullptr);
    
    QString text() const { return m_text; }
    void setText(const QString &text);
    
    QString variant() const { return m_variant; }
    void setVariant(const QString &variant);
    
    QString state() const { return m_state; }
    void setState(const QString &state);
    
    bool checked() const { return m_checked; }
    void setChecked(bool checked);
    
    bool enabled() const { return m_enabled; }
    void setEnabled(bool enabled);

signals:
    void textChanged();
    void variantChanged();
    void stateChanged();
    void checkedChanged();
    void enabledChanged();
    void clicked();
    void pressed();
    void released();

private:
    QString m_text;
    QString m_variant;
    QString m_state;
    bool m_checked;
    bool m_enabled;
};

#endif // BUTTON_H

