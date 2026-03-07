#include "messagesearch.h"
#include "messagecache.h"
#include "../core/logger.h"
#include <QRegularExpression>
#include <QtConcurrent>

MessageSearch* MessageSearch::instance()
{
    static MessageSearch instance;
    return &instance;
}

MessageSearch::MessageSearch(QObject *parent)
    : QObject(parent)
{
    Logger::instance()->debug("MessageSearch initialized", "search");
}

MessageSearch::~MessageSearch()
{
}

void MessageSearch::search(const QString &keyword)
{
    if (keyword.isEmpty() || keyword.trimmed().isEmpty()) {
        emit searchFailed("搜索关键词不能为空");
        return;
    }
    
    m_searching = true;
    m_searchResults.clear();
    
    emit searchStarted(keyword);
    Logger::instance()->info(QString("Starting search: %1").arg(keyword), "search");
    
    // 从消息缓存中搜索
    MessageCache *cache = MessageCache::instance();
    
    // 获取所有会话
    int convCount = cache->getConversationCount();
    Logger::instance()->debug(QString("Searching in %1 conversations").arg(convCount), "search");
    
    // 遍历所有会话搜索消息
    // 注意：这里需要 MessageCache 提供遍历接口，简化实现直接搜索当前会话
    
    // 简化实现：搜索所有缓存的消息
    QList<QVariantMap> results;
    
    // 使用 QtConcurrent 进行异步搜索
    auto searchTask = [this, keyword]() {
        MessageCache *cache = MessageCache::instance();
        QList<QVariantMap> results;
        
        // 这里简化实现，实际需要从缓存中遍历所有消息
        // 实际实现需要 MessageCache 支持 GetAllConversations 方法
        
        return results;
    };
    
    // 简化版本：直接返回空结果
    // 完整实现需要后端 API 支持全局消息搜索
    
    Logger::instance()->warning("Global search requires backend API support", "search");
    emit searchFinished(0);
    m_searching = false;
}

void MessageSearch::searchInConversation(const QString &conversationId, const QString &keyword)
{
    if (conversationId.isEmpty()) {
        emit searchFailed("会话 ID 不能为空");
        return;
    }
    
    if (keyword.isEmpty() || keyword.trimmed().isEmpty()) {
        emit searchFailed("搜索关键词不能为空");
        return;
    }

    m_searching = true;
    m_searchResults.clear();

    emit searchStarted(keyword);
    Logger::instance()->info(QString("Searching in conversation %1: %2").arg(conversationId).arg(keyword), "search");

    // 从缓存获取会话消息
    MessageCache *cache = MessageCache::instance();
    QList<QVariantMap> messages = cache->getMessages(conversationId, 1000);

    // 搜索匹配的消息
    QList<QVariantMap> results;  // 声明 results 变量
    for (const QVariantMap &message : messages) {
        QString content = message.value("content").toString();
        if (matchKeyword(content, keyword)) {
            QVariantMap result = message;
            result["highlighted"] = highlightText(content, keyword);
            result["conversationId"] = conversationId;
            results.append(result);
        }
    }

    m_searchResults = results;

    Logger::instance()->info(QString("Search completed: %1 results").arg(results.size()), "search");
    emit searchFinished(results.size());
    m_searching = false;
}

void MessageSearch::searchByDate(const QString &conversationId, qint64 startDate, qint64 endDate)
{
    if (conversationId.isEmpty()) {
        emit searchFailed("会话 ID 不能为空");
        return;
    }
    
    m_searching = true;
    m_searchResults.clear();
    
    emit searchStarted(QString("Date range: %1 - %2").arg(startDate).arg(endDate));
    
    // 从缓存获取消息
    MessageCache *cache = MessageCache::instance();
    QList<QVariantMap> messages = cache->getMessagesBefore(conversationId, endDate, 1000);
    
    // 过滤日期范围
    QList<QVariantMap> results;
    for (const QVariantMap &message : messages) {
        qint64 timestamp = message.value("timestamp").toLongLong();
        if (timestamp >= startDate && timestamp <= endDate) {
            results.append(message);
        }
    }
    
    m_searchResults = results;
    
    emit searchFinished(results.size());
    m_searching = false;
}

QString MessageSearch::highlightText(const QString &text, const QString &keyword, const QString &color) const
{
    if (text.isEmpty() || keyword.isEmpty()) {
        return text;
    }
    
    QString result = text;
    
    // 不区分大小写替换
    QRegularExpression re(QRegularExpression::escape(keyword), QRegularExpression::CaseInsensitiveOption);
    QString replacement = QString("<span style=\"background-color:%1;color:#000;font-weight:bold;\">\\1</span>").arg(color);
    
    // 使用捕获组
    result.replace(re, replacement);
    
    return result;
}

void MessageSearch::clear()
{
    m_searchResults.clear();
    m_searching = false;
    Logger::instance()->debug("Search cleared", "search");
}

bool MessageSearch::matchKeyword(const QString &content, const QString &keyword) const
{
    if (content.isEmpty() || keyword.isEmpty()) {
        return false;
    }
    
    // 简单包含匹配
    return content.contains(keyword, Qt::CaseInsensitive);
}

QList<QVariantMap> MessageSearch::searchFromCache(const QString &keyword) const
{
    // 从搜索历史缓存中获取结果
    // 简化实现，返回空列表
    return QList<QVariantMap>();
}
