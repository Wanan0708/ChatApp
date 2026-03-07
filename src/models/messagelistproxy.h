#ifndef MESSAGELISTPROXY_H
#define MESSAGELISTPROXY_H

#include <QObject>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

/**
 * @brief 消息列表代理模型
 * 
 * 提供消息列表虚拟化支持
 * 
 * 特性：
 * - 增量加载
 * - 缓存管理
 * - 按需加载
 * - 内存优化
 */
class MessageListProxy : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int preloadCount READ preloadCount WRITE setPreloadCount NOTIFY preloadCountChanged)
    Q_PROPERTY(int cacheSize READ cacheSize WRITE setCacheSize NOTIFY cacheSizeChanged)
    
public:
    explicit MessageListProxy(QObject *parent = nullptr);
    ~MessageListProxy() override;

    // 配置
    int preloadCount() const { return m_preloadCount; }
    void setPreloadCount(int count);
    
    int cacheSize() const { return m_cacheSize; }
    void setCacheSize(int size);
    
    // 加载控制
    Q_INVOKABLE void loadMore(int count = 20);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void clear();
    
    // 状态
    Q_INVOKABLE bool isLoading() const { return m_loading; }
    Q_INVOKABLE bool hasMore() const { return m_hasMore; }
    Q_INVOKABLE int loadedCount() const { return m_loadedCount; }

signals:
    void preloadCountChanged(int count);
    void cacheSizeChanged(int size);
    void loadingChanged(bool loading);
    void hasMoreChanged(bool hasMore);
    void loadedCountChanged(int count);
    void loadMoreRequested(int count);
    void refreshRequested();

private:
    int m_preloadCount = 20;
    int m_cacheSize = 100;
    int m_loadedCount = 0;
    bool m_loading = false;
    bool m_hasMore = true;
};

#endif // MESSAGELISTPROXY_H
