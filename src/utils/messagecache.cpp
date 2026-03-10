#include "messagecache.h"
#include "../core/logger.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDateTime>

MessageCache* MessageCache::instance()
{
    static MessageCache instance;
    return &instance;
}

MessageCache::MessageCache(QObject *parent)
    : QObject(parent)
{
    // 设置缓存目录
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_cacheDir = dataDir + "/cache";
    ensureCacheDir();
    
    Logger::instance()->info(QString("MessageCache initialized, cache dir: %1").arg(m_cacheDir), "cache");
    
    // 应用启动时自动加载
    load();
}

MessageCache::~MessageCache()
{
    // 自动保存
    if (m_dirty) {
        save();
    }
}

void MessageCache::cacheMessage(const QString &conversationId, const QVariantMap &message)
{
    if (conversationId.isEmpty() || message.isEmpty()) {
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    
    const QString internalMessageId = message.value("messageId").toString();
    const QString serverMessageId = message.value("serverMessageId").toString();
    const QString cacheMessageId = serverMessageId.isEmpty() ? internalMessageId : serverMessageId;
    if (cacheMessageId.isEmpty()) {
        return;
    }

    if (!serverMessageId.isEmpty() && !internalMessageId.isEmpty() && serverMessageId != internalMessageId) {
        m_messageCache[conversationId].remove(internalMessageId);
        m_messageOrder[conversationId].removeAll(internalMessageId);
    }
    
    // 添加到缓存
    m_messageCache[conversationId][cacheMessageId] = message;
    
    // 维护消息顺序（最新的在前面）
    if (!m_messageOrder[conversationId].contains(cacheMessageId)) {
        m_messageOrder[conversationId].prepend(cacheMessageId);
        
        // 限制缓存数量
        if (m_messageOrder[conversationId].size() > MAX_MESSAGES_PER_CONVERSATION) {
            QString oldestId = m_messageOrder[conversationId].takeLast();
            m_messageCache[conversationId].remove(oldestId);
        }
    }
    
    m_dirty = true;
    emit messageCached(conversationId, message);
}

void MessageCache::cacheMessages(const QString &conversationId, const QList<QVariantMap> &messages)
{
    if (conversationId.isEmpty() || messages.isEmpty()) {
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    
    for (const QVariantMap &message : messages) {
        const QString internalMessageId = message.value("messageId").toString();
        const QString serverMessageId = message.value("serverMessageId").toString();
        const QString cacheMessageId = serverMessageId.isEmpty() ? internalMessageId : serverMessageId;
        if (!cacheMessageId.isEmpty()) {
            if (!serverMessageId.isEmpty() && !internalMessageId.isEmpty() && serverMessageId != internalMessageId) {
                m_messageCache[conversationId].remove(internalMessageId);
                m_messageOrder[conversationId].removeAll(internalMessageId);
            }

            m_messageCache[conversationId][cacheMessageId] = message;
            
            if (!m_messageOrder[conversationId].contains(cacheMessageId)) {
                m_messageOrder[conversationId].prepend(cacheMessageId);
            }
        }
    }
    
    // 限制缓存数量
    while (m_messageOrder[conversationId].size() > MAX_MESSAGES_PER_CONVERSATION) {
        QString oldestId = m_messageOrder[conversationId].takeLast();
        m_messageCache[conversationId].remove(oldestId);
    }
    
    m_dirty = true;
}

QList<QVariantMap> MessageCache::getMessages(const QString &conversationId, int limit) const
{
    QMutexLocker locker(&m_mutex);
    
    QList<QVariantMap> result;
    
    auto orderIt = m_messageOrder.find(conversationId);
    if (orderIt == m_messageOrder.end()) {
        return result;
    }
    
    const QList<QString> &order = *orderIt;
    auto cacheIt = m_messageCache.find(conversationId);
    if (cacheIt == m_messageCache.end()) {
        return result;
    }
    
    const QHash<QString, QVariantMap> &cache = *cacheIt;
    
    for (int i = 0; i < qMin(limit, order.size()); ++i) {
        const QString &messageId = order[i];
        auto msgIt = cache.find(messageId);
        if (msgIt != cache.end()) {
            result.append(*msgIt);
        }
    }
    
    return result;
}

QList<QVariantMap> MessageCache::getMessagesBefore(const QString &conversationId, qint64 timestamp, int limit) const
{
    QMutexLocker locker(&m_mutex);
    
    QList<QVariantMap> result;
    
    auto orderIt = m_messageOrder.find(conversationId);
    if (orderIt == m_messageOrder.end()) {
        return result;
    }
    
    const QList<QString> &order = *orderIt;
    auto cacheIt = m_messageCache.find(conversationId);
    if (cacheIt == m_messageCache.end()) {
        return result;
    }
    
    const QHash<QString, QVariantMap> &cache = *cacheIt;
    
    for (const QString &messageId : order) {
        auto msgIt = cache.find(messageId);
        if (msgIt != cache.end()) {
            qint64 msgTimestamp = msgIt->value("timestamp").toLongLong();
            if (msgTimestamp < timestamp) {
                result.append(*msgIt);
                if (result.size() >= limit) {
                    break;
                }
            }
        }
    }
    
    return result;
}

QVariantMap MessageCache::getMessage(const QString &conversationId, const QString &messageId) const
{
    QMutexLocker locker(&m_mutex);
    
    auto cacheIt = m_messageCache.find(conversationId);
    if (cacheIt == m_messageCache.end()) {
        return QVariantMap();
    }
    
    auto msgIt = cacheIt->find(messageId);
    if (msgIt == cacheIt->end()) {
        for (auto it = cacheIt->cbegin(); it != cacheIt->cend(); ++it) {
            const QVariantMap &message = it.value();
            if (message.value("messageId").toString() == messageId
                || message.value("serverMessageId").toString() == messageId) {
                return message;
            }
        }
        return QVariantMap();
    }
    
    return *msgIt;
}

void MessageCache::clear()
{
    QMutexLocker locker(&m_mutex);
    m_messageCache.clear();
    m_messageOrder.clear();
    m_dirty = true;
    Logger::instance()->debug("Message cache cleared", "cache");
}

void MessageCache::clear(const QString &conversationId)
{
    QMutexLocker locker(&m_mutex);
    m_messageCache.remove(conversationId);
    m_messageOrder.remove(conversationId);
    m_dirty = true;
    Logger::instance()->debug(QString("Message cache cleared for conversation: %1").arg(conversationId), "cache");
}

void MessageCache::removeMessage(const QString &conversationId, const QString &messageId)
{
    QMutexLocker locker(&m_mutex);
    
    auto cacheIt = m_messageCache.find(conversationId);
    if (cacheIt != m_messageCache.end()) {
        cacheIt->remove(messageId);
    }
    
    auto orderIt = m_messageOrder.find(conversationId);
    if (orderIt != m_messageOrder.end()) {
        orderIt->removeAll(messageId);
    }
    
    m_dirty = true;
}

void MessageCache::save()
{
    QMutexLocker locker(&m_mutex);
    
    QString filePath = getCacheFilePath();
    QFile file(filePath);
    
    if (!file.open(QIODevice::WriteOnly)) {
        Logger::instance()->warning(QString("Failed to save message cache: %1").arg(file.errorString()), "cache");
        return;
    }
    
    QJsonObject root;
    
    // 保存消息数据
    QJsonObject conversations;
    for (auto cacheIt = m_messageCache.begin(); cacheIt != m_messageCache.end(); ++cacheIt) {
        const QString &conversationId = cacheIt.key();
        const QHash<QString, QVariantMap> &messages = cacheIt.value();
        
        QJsonObject convObj;
        for (auto msgIt = messages.begin(); msgIt != messages.end(); ++msgIt) {
            convObj[msgIt.key()] = QJsonObject::fromVariantMap(msgIt.value());
        }
        conversations[conversationId] = convObj;
    }
    root["conversations"] = conversations;
    
    // 保存消息顺序
    QJsonObject order;
    for (auto orderIt = m_messageOrder.begin(); orderIt != m_messageOrder.end(); ++orderIt) {
        QJsonArray arr;
        for (const QString &id : orderIt.value()) {
            arr.append(id);
        }
        order[orderIt.key()] = arr;
    }
    root["order"] = order;
    
    root["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    
    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
    
    m_dirty = false;
    Logger::instance()->debug(QString("Message cache saved to: %1").arg(filePath), "cache");
    emit cacheSaved();
}

void MessageCache::load()
{
    QMutexLocker locker(&m_mutex);
    
    QString filePath = getCacheFilePath();
    QFile file(filePath);
    
    if (!file.exists()) {
        Logger::instance()->debug("No message cache file found", "cache");
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        Logger::instance()->warning(QString("Failed to load message cache: %1").arg(file.errorString()), "cache");
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        Logger::instance()->warning(QString("Failed to parse message cache: %1").arg(error.errorString()), "cache");
        return;
    }
    
    QJsonObject root = doc.object();
    
    // 加载消息数据
    QJsonObject conversations = root["conversations"].toObject();
    for (auto it = conversations.begin(); it != conversations.end(); ++it) {
        const QString &conversationId = it.key();
        QJsonObject convObj = it.value().toObject();

        QHash<QString, QVariantMap> messages;
        for (auto msgIt = convObj.begin(); msgIt != convObj.end(); ++msgIt) {
            messages[msgIt.key()] = msgIt.value().toVariant().toMap();
        }
        m_messageCache[conversationId] = messages;
    }
    
    // 加载消息顺序
    QJsonObject order = root["order"].toObject();
    for (auto it = order.begin(); it != order.end(); ++it) {
        QList<QString> msgOrder;
        QJsonArray arr = it.value().toArray();
        for (const QJsonValue &val : arr) {
            msgOrder.append(val.toString());
        }
        m_messageOrder[it.key()] = msgOrder;
    }
    
    Logger::instance()->debug(QString("Message cache loaded from: %1").arg(filePath), "cache");
    emit cacheLoaded();
}

int MessageCache::getConversationCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_messageCache.size();
}

int MessageCache::getMessageCount(const QString &conversationId) const
{
    QMutexLocker locker(&m_mutex);
    
    auto it = m_messageCache.find(conversationId);
    if (it == m_messageCache.end()) {
        return 0;
    }
    return it.value().size();
}

qint64 MessageCache::getCacheSize() const
{
    QString filePath = getCacheFilePath();
    QFileInfo info(filePath);
    if (info.exists()) {
        return info.size();
    }
    return 0;
}

QString MessageCache::getCacheFilePath() const
{
    return m_cacheDir + "/messages.json";
}

void MessageCache::ensureCacheDir()
{
    QDir().mkpath(m_cacheDir);
}
