#include "fallbackstyling.h"
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

FallbackStyling::FallbackStyling(QObject* parent)
    : QObject(parent)
    , m_active(false)
    , m_reason("")
{
    checkXenonEffects();
}

FallbackStyling::~FallbackStyling()
{
}

void FallbackStyling::checkXenonEffects()
{
    if (!canRenderXenonEffects()) {
        activateFallback("Xenon effects cannot render with current theme/graphics configuration");
    } else {
        deactivateFallback();
    }
}

bool FallbackStyling::canRenderXenonEffects()
{
    // Check if OpenGL is available (required for xenon effects)
    QOpenGLContext context;
    if (!context.create()) {
        return false;
    }
    
    QOpenGLFunctions* gl = context.functions();
    if (!gl) {
        return false;
    }
    
    // Check for required OpenGL extensions
    QString extensions = QString::fromLatin1(reinterpret_cast<const char*>(gl->glGetString(GL_EXTENSIONS)));
    if (!extensions.contains("GL_ARB_shader_objects")) {
        return false;
    }
    
    return true;
}

void FallbackStyling::activateFallback(const QString& reason)
{
    if (!m_active) {
        m_active = true;
        m_reason = reason;
        emit activeChanged();
        emit reasonChanged();
        emit fallbackActivated(reason);
    }
}

void FallbackStyling::deactivateFallback()
{
    if (m_active) {
        m_active = false;
        m_reason = "";
        emit activeChanged();
        emit reasonChanged();
    }
}

QColor FallbackStyling::getFallbackColor(const QString& componentType)
{
    // High-contrast colors for fallback styling
    if (componentType == "button") {
        return QColor("#0066cc"); // High-contrast blue
    } else if (componentType == "border") {
        return QColor("#000000"); // Black border
    } else if (componentType == "text") {
        return QColor("#000000"); // Black text
    } else if (componentType == "background") {
        return QColor("#ffffff"); // White background
    } else if (componentType == "hover") {
        return QColor("#003399"); // Darker blue for hover
    }
    return QColor("#000000"); // Default black
}

int FallbackStyling::getFallbackBorderWidth(const QString& componentType)
{
    // Thicker borders for better visibility
    if (componentType == "button") {
        return 3;
    } else if (componentType == "card") {
        return 2;
    } else if (componentType == "input") {
        return 2;
    }
    return 2; // Default
}

int FallbackStyling::getFallbackFontSize(const QString& componentType)
{
    // Larger fonts for better readability
    if (componentType == "button") {
        return 14;
    } else if (componentType == "heading") {
        return 18;
    } else if (componentType == "body") {
        return 12;
    }
    return 12; // Default
}

