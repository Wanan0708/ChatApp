#ifndef MESSAGESEARCH_H
#define MESSAGESEARCH_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>

/**
 * @brief 消息搜索器
 * 
 * 提供消息搜索功能
 * 
 * 特性：
 * - 关键词搜索
 * - 按会话搜索
 * - 按时间范围搜索
 * - 高亮显示
 * - 搜索结果缓存
 */
class MessageSearch : public QObject
{
    Q_OBJECT
public:
    static MessageSearch* instance();

    // 搜索方法
    Q_INVOKABLE void search(const QString &keyword);
    Q_INVOKABLE void searchInConversation(const QString &conversationId, const QString &keyword);
    Q_INVOKABLE void searchByDate(const QString &conversationId, qint64 startDate, qint64 endDate);
    
    // 获取搜索结果
    Q_INVOKABLE QList<QVariantMap> getResults() const { return m_searchResults; }
    Q_INVOKABLE int getResultCount() const { return m_searchResults.size(); }
    
    // 高亮文本
    Q_INVOKABLE QString highlightText(const QString &text, const QString &keyword, const QString &color = "#f59e0b") const;
    
    // 清除搜索
    Q_INVOKABLE void clear();

signals:
    void searchStarted(const QString &keyword);
    void searchFinished(int count);
    void searchFailed(const QString &error);

private:
    explicit MessageSearch(QObject *parent = nullptr);
    ~MessageSearch() override;

    bool matchKeyword(const QString &content, const QString &keyword) const;
    QList<QVariantMap> searchFromCache(const QString &keyword) const;

    QList<QVariantMap> m_searchResults;
    bool m_searching = false;
};

#endif // MESSAGESEARCH_H
