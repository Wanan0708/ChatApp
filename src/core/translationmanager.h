#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QSettings>

/**
 * @brief 翻译管理器
 * 
 * 提供应用多语言支持
 * 
 * 特性：
 * - 多语言切换
 * - 运行时动态切换
 * - 持久化保存
 * - 回退机制
 */
class TranslationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY currentLanguageChanged)
    
public:
    static TranslationManager* instance();

    // 当前语言
    Q_INVOKABLE QString currentLanguage() const { return m_currentLanguage; }
    Q_INVOKABLE void setCurrentLanguage(const QString &language);
    
    // 翻译文本
    Q_INVOKABLE QString tr(const QString &key) const;
    
    // 语言列表
    Q_INVOKABLE QStringList availableLanguages() const { return m_languages.keys(); }
    
    // 语言名称
    Q_INVOKABLE QString languageName(const QString &code) const;
    
    // 保存/加载
    Q_INVOKABLE void save();
    Q_INVOKABLE void load();

signals:
    void currentLanguageChanged(const QString &language);
    void translationChanged();

private:
    explicit TranslationManager(QObject *parent = nullptr);
    ~TranslationManager() override;

    void initializeTranslations();

    QHash<QString, QHash<QString, QString>> m_languages;
    QString m_currentLanguage;
    QSettings *m_settings;
};

// 便捷宏
#define TR(key) TranslationManager::instance()->tr(key)

#endif // TRANSLATIONMANAGER_H
