#include "thememanager.h"
#include "logger.h"

ThemeManager* ThemeManager::instance()
{
    static ThemeManager instance;
    return &instance;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings(this))
{
    initializeThemes();
    load();
    
    Logger::instance()->info(QString("ThemeManager initialized with theme: %1").arg(m_currentTheme), "theme");
}

ThemeManager::~ThemeManager()
{
    save();
}

void ThemeManager::initializeThemes()
{
    // 浅色主题
    QVariantMap lightColors;
    lightColors["primary"] = "#4f46e5";
    lightColors["primaryDark"] = "#4338ca";
    lightColors["secondary"] = "#7c3aed";
    lightColors["background"] = "#ffffff";
    lightColors["surface"] = "#f8fafc";
    lightColors["error"] = "#ef4444";
    lightColors["success"] = "#22c55e";
    lightColors["warning"] = "#f59e0b";
    lightColors["textPrimary"] = "#1e293b";
    lightColors["textSecondary"] = "#64748b";
    lightColors["textDisabled"] = "#94a3b8";
    lightColors["border"] = "#e2e8f0";
    lightColors["divider"] = "#f1f5f9";
    lightColors["chatBubbleSelf"] = "#DCF8C6";
    lightColors["chatBubbleOther"] = "#ECECEC";
    m_themes["light"] = lightColors;
    
    // 深色主题
    QVariantMap darkColors;
    darkColors["primary"] = "#6366f1";
    darkColors["primaryDark"] = "#4f46e5";
    darkColors["secondary"] = "#8b5cf6";
    darkColors["background"] = "#0f172a";
    darkColors["surface"] = "#1e293b";
    darkColors["error"] = "#f87171";
    darkColors["success"] = "#4ade80";
    darkColors["warning"] = "#fbbf24";
    darkColors["textPrimary"] = "#f1f5f9";
    darkColors["textSecondary"] = "#94a3b8";
    darkColors["textDisabled"] = "#64748b";
    darkColors["border"] = "#334155";
    darkColors["divider"] = "#1e293b";
    darkColors["chatBubbleSelf"] = "#166534";
    darkColors["chatBubbleOther"] = "#1e293b";
    m_themes["dark"] = darkColors;
    
    // 默认主题
    m_currentTheme = "light";
}

void ThemeManager::setCurrentTheme(const QString &theme)
{
    if (!m_themes.contains(theme)) {
        Logger::instance()->warning(QString("Unknown theme: %1").arg(theme), "theme");
        return;
    }
    
    if (m_currentTheme != theme) {
        m_currentTheme = theme;
        Logger::instance()->info(QString("Theme changed to: %1").arg(theme), "theme");
        emit currentThemeChanged(theme);
        emit currentColorsChanged();
        save();
    }
}

QVariantMap ThemeManager::getThemeColors(const QString &theme) const
{
    auto it = m_themes.find(theme);
    if (it != m_themes.end()) {
        return *it;
    }
    return m_themes.value("light");
}

void ThemeManager::setThemeColor(const QString &theme, const QString &key, const QString &color)
{
    if (m_themes.contains(theme)) {
        m_themes[theme][key] = color;
        emit colorChanged(key, color);
        
        if (theme == m_currentTheme) {
            emit currentColorsChanged();
        }
        
        save();
    }
}

void ThemeManager::toggleTheme()
{
    if (m_currentTheme == "light") {
        setCurrentTheme("dark");
    } else {
        setCurrentTheme("light");
    }
}

QString ThemeManager::getColor(const QString &name) const
{
    auto colors = currentColors();
    if (colors.contains(name)) {
        return colors[name].toString();
    }
    return "#000000";
}

void ThemeManager::save()
{
    m_settings->setValue("theme/current", m_currentTheme);
    m_settings->sync();
    Logger::instance()->debug("Theme settings saved", "theme");
}

void ThemeManager::load()
{
    QString savedTheme = m_settings->value("theme/current", "light").toString();
    if (m_themes.contains(savedTheme)) {
        m_currentTheme = savedTheme;
        Logger::instance()->debug(QString("Loaded theme: %1").arg(savedTheme), "theme");
    }
}
