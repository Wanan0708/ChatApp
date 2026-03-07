#ifndef REQUESTBATCHER_H
#define REQUESTBATCHER_H

#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QJsonObject>
#include <QMutex>
#include <functional>

/**
 * @brief 请求批处理器
 * 
 * 合并短时间内多个相同类型的请求，减少网络开销
 * 
 * 特性：
 * - 请求去重
 * - 批量发送
 * - 延迟合并
 * - 优先级队列
 */
class RequestBatcher : public QObject
{
    Q_OBJECT
public:
    static RequestBatcher* instance();

    // 请求结构
    struct Request {
        QString path;
        QString method;  // GET, POST
        QJsonObject data;
        int priority;    // 优先级：0=普通，1=高，2=紧急
        qint64 timestamp;
        
        // 回调
        std::function<void(const QJsonObject&)> onSuccess;
        std::function<void(const QString&)> onError;
        
        bool operator==(const Request &other) const {
            return path == other.path && method == other.method && data == other.data;
        }
    };

    // 添加请求到队列
    void enqueue(const QString &path, const QJsonObject &data,
                 std::function<void(const QJsonObject&)> onSuccess,
                 std::function<void(const QString&)> onError = nullptr,
                 const QString &method = "POST",
                 int priority = 0);

    // 配置
    void setBatchInterval(int msec) { m_batchInterval = msec; }
    void setMaxBatchSize(int size) { m_maxBatchSize = size; }
    int batchInterval() const { return m_batchInterval; }
    int maxBatchSize() const { return m_maxBatchSize; }

    // 立即发送所有待处理请求
    Q_INVOKABLE void flush();

signals:
    void batchReady(const QList<Request> &requests);
    void requestDequeued(const QString &path);

private slots:
    void processBatch();

private:
    explicit RequestBatcher(QObject *parent = nullptr);
    ~RequestBatcher() override;

    bool containsDuplicate(const Request &request) const;

    QQueue<Request> m_requestQueue;
    QList<Request> m_currentBatch;
    QTimer *m_batchTimer;
    QMutex m_mutex;
    
    int m_batchInterval = 100;      // 100ms 合并窗口
    int m_maxBatchSize = 10;        // 最大批量大小
    bool m_processing = false;
};

#endif // REQUESTBATCHER_H
