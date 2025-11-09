#ifndef INPUTFIELD_H
#define INPUTFIELD_H

#include <QObject>
#include <QQuickItem>

class InputField : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString placeholder READ placeholder WRITE setPlaceholder NOTIFY placeholderChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage WRITE setErrorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(int passwordStrength READ passwordStrength WRITE setPasswordStrength NOTIFY passwordStrengthChanged)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength NOTIFY maxLengthChanged)

public:
    explicit InputField(QQuickItem *parent = nullptr);
    
    QString variant() const { return m_variant; }
    void setVariant(const QString &variant);
    
    QString state() const { return m_state; }
    void setState(const QString &state);
    
    QString placeholder() const { return m_placeholder; }
    void setPlaceholder(const QString &placeholder);
    
    QString label() const { return m_label; }
    void setLabel(const QString &label);
    
    QString value() const { return m_value; }
    void setValue(const QString &value);
    
    QString errorMessage() const { return m_errorMessage; }
    void setErrorMessage(const QString &errorMessage);
    
    bool enabled() const { return m_enabled; }
    void setEnabled(bool enabled);
    
    bool readOnly() const { return m_readOnly; }
    void setReadOnly(bool readOnly);
    
    int passwordStrength() const { return m_passwordStrength; }
    void setPasswordStrength(int strength);
    
    int maxLength() const { return m_maxLength; }
    void setMaxLength(int maxLength);

signals:
    void variantChanged();
    void stateChanged();
    void placeholderChanged();
    void labelChanged();
    void valueChanged();
    void errorMessageChanged();
    void enabledChanged();
    void readOnlyChanged();
    void passwordStrengthChanged();
    void maxLengthChanged();

private:
    QString m_variant;
    QString m_state;
    QString m_placeholder;
    QString m_label;
    QString m_value;
    QString m_errorMessage;
    bool m_enabled;
    bool m_readOnly;
    int m_passwordStrength;
    int m_maxLength;
};

#endif // INPUTFIELD_H

