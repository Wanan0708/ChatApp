#ifndef EMOJIPROVIDER_H
#define EMOJIPROVIDER_H

#include <QObject>
#include <QList>
#include <QVariantMap>
#include <QHash>

/**
 * @brief 表情符号数据结构
 */
struct Emoji {
    QString code;       // 表情代码
    QString emoji;      // 表情字符
    QString name;       // 表情名称
    QString category;   // 分类
    QString tags;       // 标签
};

/**
 * @brief 表情提供者
 * 
 * 提供表情符号选择和插入功能
 * 
 * 特性：
 * - 常用表情
 * - 分类浏览
 * - 搜索表情
 * - 最近使用
 */
class EmojiProvider : public QObject
{
    Q_OBJECT
public:
    static EmojiProvider* instance();

    // 获取表情列表
    Q_INVOKABLE QVariantList getEmojis(const QString &category = "") const;
    Q_INVOKABLE QVariantList getRecentEmojis() const { return m_recentEmojis; }
    Q_INVOKABLE QVariantList getFavoriteEmojis() const { return m_favoriteEmojis; }
    
    // 分类
    Q_INVOKABLE QStringList getCategories() const;
    
    // 搜索表情
    Q_INVOKABLE QVariantList searchEmojis(const QString &keyword) const;
    
    // 表情操作
    Q_INVOKABLE void addRecent(const QString &emojiCode);
    Q_INVOKABLE void toggleFavorite(const QString &emojiCode);
    Q_INVOKABLE bool isFavorite(const QString &emojiCode) const;
    
    // 表情转义
    Q_INVOKABLE QString emojiToUnicode(const QString &code) const;
    Q_INVOKABLE QString unicodeToEmoji(const QString &unicode) const;
    
    // 解析消息中的表情
    Q_INVOKABLE QString parseEmojis(const QString &text) const;

signals:
    void recentChanged();
    void favoriteChanged();

private:
    explicit EmojiProvider(QObject *parent = nullptr);
    ~EmojiProvider() override;

    void initializeEmojis();
    const Emoji* findEmoji(const QString &code) const;

    QHash<QString, Emoji> m_emojis;
    QVariantList m_recentEmojis;
    QVariantList m_favoriteEmojis;
    QStringList m_categories;
    
    static const int MAX_RECENT = 20;
};

#endif // EMOJIPROVIDER_H
