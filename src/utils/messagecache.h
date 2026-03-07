#ifndef MESSAGECACHE_H
#define MESSAGECACHE_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QVariantMap>
#include <QMutex>
#include <QFile>
#include <QDir>

/**
 * @brief 消息本地缓存
 * 
 * 提供消息的本地持久化缓存
 * 
 * 特性：
 * - 按会话缓存消息
 * - 自动持久化
 * - 批量加载
 * - 过期清理
 */
class MessageCache : public QObject
{
    Q_OBJECT
public:
    static MessageCache* instance();

    // 消息存储
    Q_INVOKABLE void cacheMessage(const QString &conversationId, const QVariantMap &message);
    Q_INVOKABLE void cacheMessages(const QString &conversationId, const QList<QVariantMap> &messages);
    
    // 消息查询
    Q_INVOKABLE QList<QVariantMap> getMessages(const QString &conversationId, int limit = 50) const;
    Q_INVOKABLE QList<QVariantMap> getMessagesBefore(const QString &conversationId, qint64 timestamp, int limit = 50) const;
    Q_INVOKABLE QVariantMap getMessage(const QString &conversationId, const QString &messageId) const;
    
    // 缓存管理
    Q_INVOKABLE void clear();
    Q_INVOKABLE void clear(const QString &conversationId);
    Q_INVOKABLE void removeMessage(const QString &conversationId, const QString &messageId);
    Q_INVOKABLE void save();  // 持久化到磁盘
    Q_INVOKABLE void load();  // 从磁盘加载
    
    // 统计信息
    Q_INVOKABLE int getConversationCount() const;
    Q_INVOKABLE int getMessageCount(const QString &conversationId) const;
    Q_INVOKABLE qint64 getCacheSize() const;

signals:
    void messageCached(const QString &conversationId, const QVariantMap &message);
    void cacheSaved();
    void cacheLoaded();

private:
    explicit MessageCache(QObject *parent = nullptr);
    ~MessageCache() override;

    QString getCacheFilePath() const;
    void ensureCacheDir();

    // 缓存结构：conversationId -> (messageId -> message)
    QHash<QString, QHash<QString, QVariantMap>> m_messageCache;
    QHash<QString, QList<QString>> m_messageOrder;  // 保持消息顺序
    
    mutable QMutex m_mutex;
    QString m_cacheDir;
    bool m_dirty = false;
    
    static const int MAX_MESSAGES_PER_CONVERSATION = 500;  // 每个会话最多缓存 500 条
};

#endif // MESSAGECACHE_H
