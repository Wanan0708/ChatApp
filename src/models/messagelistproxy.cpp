#include "messagelistproxy.h"
#include "../core/logger.h"

MessageListProxy::MessageListProxy(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    Logger::instance()->debug("MessageListProxy initialized", "proxy");
}

MessageListProxy::~MessageListProxy()
{
}

void MessageListProxy::setPreloadCount(int count)
{
    if (m_preloadCount != count) {
        m_preloadCount = count;
        emit preloadCountChanged(count);
    }
}

void MessageListProxy::setCacheSize(int size)
{
    if (m_cacheSize != size) {
        m_cacheSize = size;
        emit cacheSizeChanged(size);
        
        // 如果当前加载数量超过新缓存大小，清理多余数据
        if (m_loadedCount > m_cacheSize) {
            // 触发模型清理
            refresh();
        }
    }
}

void MessageListProxy::loadMore(int count)
{
    if (m_loading || !m_hasMore) {
        return;
    }
    
    m_loading = true;
    emit loadingChanged(true);
    
    Logger::instance()->debug(QString("Loading more %1 messages").arg(count), "proxy");
    
    // 发出加载更多请求
    emit loadMoreRequested(count);
}

void MessageListProxy::refresh()
{
    Logger::instance()->debug("Refreshing message list", "proxy");
    
    m_loadedCount = 0;
    m_hasMore = true;
    m_loading = false;
    
    emit loadedCountChanged(0);
    emit hasMoreChanged(true);
    emit loadingChanged(false);
    emit refreshRequested();
}

void MessageListProxy::clear()
{
    Logger::instance()->debug("Clearing message list", "proxy");
    
    m_loadedCount = 0;
    m_hasMore = true;
    m_loading = false;
    
    emit loadedCountChanged(0);
    emit hasMoreChanged(true);
    emit loadingChanged(false);
}
