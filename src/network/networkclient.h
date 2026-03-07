#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrl>
#include <QPointer>
#include <QFile>
#include <QHttpMultiPart>
#include <QTimer>
#include <functional>

/**
 * @brief 网络客户端（单例模式）
 * 
 * 负责与后端 API 通信，包括 HTTP 请求和文件上传
 * 
 * 特性：
 * - 自动重试（指数退避）
 * - 请求超时控制
 * - 请求队列
 * - 统一错误处理
 */
class NetworkClient : public QObject
{
    Q_OBJECT
public:
    static NetworkClient* instance();

    // 回调函数类型定义
    typedef std::function<void(const QJsonObject&)> SuccessHandler;
    typedef std::function<void(const QJsonArray&)> ArraySuccessHandler;
    typedef std::function<void(const QString&)> ErrorHandler;
    typedef std::function<void(const QJsonObject&, qint64, qint64)> UploadProgressHandler;

    // 基础配置
    void setBaseUrl(const QString &url);
    void setToken(const QString &token);
    QString token() const { return m_token; }

    // GET 请求
    void get(const QString &path, SuccessHandler onSuccess, ErrorHandler onError = nullptr);
    void getArray(const QString &path, ArraySuccessHandler onSuccess, ErrorHandler onError = nullptr);

    // POST 请求
    void post(const QString &path, const QJsonObject &data, SuccessHandler onSuccess, ErrorHandler onError = nullptr);

    // PUT 请求
    void put(const QString &path, const QJsonObject &data, SuccessHandler onSuccess, ErrorHandler onError = nullptr);

    // 文件上传
    QNetworkReply* uploadFile(const QString &path, const QString &filePath, 
                              const QString &fileFieldName = "file",
                              const QJsonObject &extraData = QJsonObject(),
                              UploadProgressHandler onProgress = nullptr,
                              SuccessHandler onSuccess = nullptr, 
                              ErrorHandler onError = nullptr);

    // 重试和超时配置
    void setMaxRetryAttempts(int attempts) { m_maxRetryAttempts = attempts; }
    void setRequestTimeout(int msec) { m_requestTimeout = msec; }
    int maxRetryAttempts() const { return m_maxRetryAttempts; }
    int requestTimeout() const { return m_requestTimeout; }

signals:
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void uploadFinished(const QString &fileId, const QString &fileUrl);
    void uploadFailed(const QString &error);
    void retryAttempted(int attempt, int maxAttempts);
    void requestTimeoutOccurred(const QString &path);

private:
    explicit NetworkClient(QObject *parent = nullptr);
    QNetworkRequest createRequest(const QString &path);
    void handleReply(QNetworkReply *reply, SuccessHandler onSuccess, ErrorHandler onError, 
                     bool isArray = false, ArraySuccessHandler onArraySuccess = nullptr);
    void retryRequest(const QString &path, const QJsonObject &data, const QString &method,
                      SuccessHandler onSuccess, ErrorHandler onError, int attempt);

    QNetworkAccessManager *m_manager;
    QString m_baseUrl;
    QString m_token;
    
    // 重试和超时配置
    int m_maxRetryAttempts = 3;
    int m_requestTimeout = 30000; // 30 秒
};

#endif // NETWORKCLIENT_H
