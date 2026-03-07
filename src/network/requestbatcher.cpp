#include "requestbatcher.h"
#include "networkclient.h"
#include "../core/logger.h"
#include <QJsonArray>

RequestBatcher* RequestBatcher::instance()
{
    static RequestBatcher instance;
    return &instance;
}

RequestBatcher::RequestBatcher(QObject *parent)
    : QObject(parent)
    , m_batchTimer(new QTimer(this))
{
    connect(m_batchTimer, &QTimer::timeout, this, &RequestBatcher::processBatch);
    m_batchTimer->setSingleShot(true);
    Logger::instance()->debug("RequestBatcher initialized", "network");
}

RequestBatcher::~RequestBatcher()
{
}

void RequestBatcher::enqueue(const QString &path, const QJsonObject &data,
                              std::function<void(const QJsonObject&)> onSuccess,
                              std::function<void(const QString&)> onError,
                              const QString &method,
                              int priority)
{
    QMutexLocker locker(&m_mutex);
    
    Request request;
    request.path = path;
    request.method = method;
    request.data = data;
    request.priority = priority;
    request.timestamp = QDateTime::currentMSecsSinceEpoch();
    request.onSuccess = onSuccess;
    request.onError = onError;
    
    // 检查是否已存在相同请求（去重）
    if (containsDuplicate(request)) {
        Logger::instance()->debug(QString("Duplicate request skipped: %1").arg(path), "network");
        return;
    }
    
    // 高优先级请求立即发送
    if (priority >= 2) {
        m_batchTimer->stop();
        locker.unlock();
        
        // 直接发送
        if (method == "GET") {
            NetworkClient::instance()->get(path, onSuccess, onError);
        } else {
            NetworkClient::instance()->post(path, data, onSuccess, onError);
        }
        return;
    }
    
    // 添加到队列
    m_requestQueue.enqueue(request);
    Logger::instance()->debug(QString("Request queued: %1 (queue size: %2)").arg(path).arg(m_requestQueue.size()), "network");
    
    // 如果达到最大批量大小，立即发送
    if (m_requestQueue.size() >= m_maxBatchSize) {
        m_batchTimer->stop();
        QTimer::singleShot(0, this, &RequestBatcher::processBatch);
        return;
    }
    
    // 启动合并定时器
    if (!m_batchTimer->isActive()) {
        m_batchTimer->start(m_batchInterval);
    }
}

void RequestBatcher::flush()
{
    QMutexLocker locker(&m_mutex);
    m_batchTimer->stop();
    locker.unlock();
    
    processBatch();
}

void RequestBatcher::processBatch()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_requestQueue.isEmpty() || m_processing) {
        return;
    }
    
    m_processing = true;
    m_currentBatch.clear();
    
    // 从队列中取出一批请求
    while (!m_requestQueue.isEmpty() && m_currentBatch.size() < m_maxBatchSize) {
        m_currentBatch.append(m_requestQueue.dequeue());
    }
    
    locker.unlock();
    
    Logger::instance()->debug(QString("Processing batch of %1 requests").arg(m_currentBatch.size()), "network");
    
    // 合并相同路径的请求
    QHash<QString, QJsonArray> batchedData;
    QHash<QString, QList<std::function<void(const QJsonObject&)>>> batchCallbacks;
    QHash<QString, QList<std::function<void(const QString&)>>> errorCallbacks;
    
    for (const Request &req : m_currentBatch) {
        if (req.method == "POST") {
            batchedData[req.path].append(req.data);
            batchCallbacks[req.path].append(req.onSuccess);
            errorCallbacks[req.path].append(req.onError);
        } else {
            // GET 请求单独发送
            if (req.method == "GET") {
                NetworkClient::instance()->get(req.path, req.onSuccess, req.onError);
            }
        }
    }
    
    // 发送合并后的请求
    for (auto it = batchedData.begin(); it != batchedData.end(); ++it) {
        const QString &path = it.key();
        QJsonArray &dataArray = it.value();
        
        QJsonObject batchRequest;
        batchRequest["batch"] = true;
        batchRequest["requests"] = dataArray;
        
        // 合并回调
        auto onSuccesses = batchCallbacks[path];
        auto onErrors = errorCallbacks[path];
        
        NetworkClient::instance()->post(path, batchRequest, 
            [onSuccesses, onErrors, this](const QJsonObject &response) {
                // 分发响应给所有回调
                QJsonArray results = response["results"].toArray();
                for (int i = 0; i < results.size() && i < onSuccesses.size(); ++i) {
                    if (onSuccesses[i]) {
                        onSuccesses[i](results[i].toObject());
                    }
                }
            },
            [onErrors, this](const QString &error) {
                // 分发错误给所有回调
                for (const auto &onError : onErrors) {
                    if (onError) {
                        onError(error);
                    }
                }
            }
        );
    }
    
    m_currentBatch.clear();
    m_processing = false;
}

bool RequestBatcher::containsDuplicate(const Request &request) const
{
    for (const Request &queued : m_requestQueue) {
        if (queued == request) {
            return true;
        }
    }
    return false;
}
