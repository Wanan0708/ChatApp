#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QVariantMap>
#include <QSettings>
#include <QHash>

/**
 * @brief 主题管理器
 * 
 * 提供应用主题切换功能
 * 
 * 特性：
 * - 浅色/深色主题
 * - 自定义主题色
 * - 持久化保存
 * - 实时切换
 */
class ThemeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)
    Q_PROPERTY(QVariantMap currentColors READ currentColors NOTIFY currentColorsChanged)
    
public:
    static ThemeManager* instance();

    // 主题名称
    Q_INVOKABLE QString currentTheme() const { return m_currentTheme; }
    Q_INVOKABLE void setCurrentTheme(const QString &theme);
    
    // 获取当前主题颜色
    Q_INVOKABLE QVariantMap currentColors() const { return getThemeColors(m_currentTheme); }
    
    // 获取指定主题颜色
    Q_INVOKABLE QVariantMap getThemeColors(const QString &theme) const;
    
    // 设置主题颜色
    Q_INVOKABLE void setThemeColor(const QString &theme, const QString &key, const QString &color);
    
    // 主题切换
    Q_INVOKABLE void toggleTheme();
    
    // 主题列表
    Q_INVOKABLE QStringList availableThemes() const { return m_themes.keys(); }
    
    // 获取特定颜色值
    Q_INVOKABLE QString getColor(const QString &name) const;
    
    // 保存/加载
    Q_INVOKABLE void save();
    Q_INVOKABLE void load();

signals:
    void currentThemeChanged(const QString &theme);
    void currentColorsChanged();
    void colorChanged(const QString &name, const QString &value);

private:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager() override;

    void initializeThemes();

    QHash<QString, QVariantMap> m_themes;
    QString m_currentTheme;
    QSettings *m_settings;
};

#endif // THEMEMANAGER_H
