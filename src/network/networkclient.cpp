#include "networkclient.h"
#include "../database/databaseconfig.h"
#include "../core/logger.h"
#include <QDebug>
#include <QTimer>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>

NetworkClient* NetworkClient::instance()
{
    static NetworkClient *inst = new NetworkClient();
    return inst;
}

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_baseUrl(DatabaseConfig::apiUrl())
{
    Logger::instance()->info(QString("NetworkClient initialized with Base URL: %1").arg(m_baseUrl), "network");
}

void NetworkClient::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
    Logger::instance()->debug(QString("Base URL changed to: %1").arg(url), "network");
}

void NetworkClient::setToken(const QString &token)
{
    m_token = token;
}

QNetworkRequest NetworkClient::createRequest(const QString &path)
{
    QUrl url(m_baseUrl + path);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (!m_token.isEmpty()) {
        request.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());
    }

    return request;
}

void NetworkClient::get(const QString &path, SuccessHandler onSuccess, ErrorHandler onError)
{
    QNetworkReply *reply = m_manager->get(createRequest(path));
    
    // 设置超时
    QTimer *timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->start(m_requestTimeout);
    
    connect(timeoutTimer, &QTimer::timeout, this, [this, reply, timeoutTimer, path, onError]() {
        if (reply && reply->isRunning()) {
            reply->abort();
            Logger::instance()->warning(QString("Request timeout: %1").arg(path), "network");
            emit requestTimeoutOccurred(path);
            if (onError) onError("请求超时");
        }
        timeoutTimer->deleteLater();
    });
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, timeoutTimer, onSuccess, onError]() {
        timeoutTimer->stop();
        timeoutTimer->deleteLater();
        handleReply(reply, onSuccess, onError);
    });
}

void NetworkClient::getArray(const QString &path, ArraySuccessHandler onSuccess, ErrorHandler onError)
{
    QNetworkReply *reply = m_manager->get(createRequest(path));
    
    // 设置超时
    QTimer *timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->start(m_requestTimeout);
    
    connect(timeoutTimer, &QTimer::timeout, this, [this, reply, timeoutTimer, path, onError]() {
        if (reply && reply->isRunning()) {
            reply->abort();
            Logger::instance()->warning(QString("Request timeout: %1").arg(path), "network");
            emit requestTimeoutOccurred(path);
            if (onError) onError("请求超时");
        }
        timeoutTimer->deleteLater();
    });
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, timeoutTimer, onSuccess, onError]() {
        timeoutTimer->stop();
        timeoutTimer->deleteLater();
        handleReply(reply, nullptr, onError, true, onSuccess);
    });
}

void NetworkClient::post(const QString &path, const QJsonObject &data, SuccessHandler onSuccess, ErrorHandler onError)
{
    QNetworkRequest request = createRequest(path);
    QNetworkReply *reply = m_manager->post(request, QJsonDocument(data).toJson());
    
    // 设置超时
    QTimer *timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->start(m_requestTimeout);
    
    connect(timeoutTimer, &QTimer::timeout, this, [this, reply, timeoutTimer, path, onError]() {
        if (reply && reply->isRunning()) {
            reply->abort();
            Logger::instance()->warning(QString("Request timeout: %1").arg(path), "network");
            emit requestTimeoutOccurred(path);
            if (onError) onError("请求超时");
        }
        timeoutTimer->deleteLater();
    });
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, timeoutTimer, onSuccess, onError]() {
        timeoutTimer->stop();
        timeoutTimer->deleteLater();
        handleReply(reply, onSuccess, onError);
    });
}

void NetworkClient::put(const QString &path, const QJsonObject &data, SuccessHandler onSuccess, ErrorHandler onError)
{
    QNetworkReply *reply = m_manager->put(createRequest(path), QJsonDocument(data).toJson());
    
    // 设置超时
    QTimer *timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->start(m_requestTimeout);
    
    connect(timeoutTimer, &QTimer::timeout, this, [this, reply, timeoutTimer, path, onError]() {
        if (reply && reply->isRunning()) {
            reply->abort();
            Logger::instance()->warning(QString("Request timeout: %1").arg(path), "network");
            emit requestTimeoutOccurred(path);
            if (onError) onError("请求超时");
        }
        timeoutTimer->deleteLater();
    });
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, timeoutTimer, onSuccess, onError]() {
        timeoutTimer->stop();
        timeoutTimer->deleteLater();
        handleReply(reply, onSuccess, onError);
    });
}

QNetworkReply* NetworkClient::uploadFile(const QString &path, const QString &filePath,
                                          const QString &fileFieldName,
                                          const QJsonObject &extraData,
                                          UploadProgressHandler onProgress,
                                          SuccessHandler onSuccess,
                                          ErrorHandler onError)
{
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        Logger::instance()->error(QString("Failed to open file: %1").arg(filePath), "network");
        delete file;
        if (onError) onError("无法打开文件");
        return nullptr;
    }

    // 创建 multipart 表单
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    
    // 添加文件部分
    QHttpPart filePart;
    QMimeDatabase mimeDatabase;
    const QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);
    const QString contentType = mimeType.isValid() && !mimeType.name().isEmpty()
        ? mimeType.name()
        : QStringLiteral("application/octet-stream");
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, 
                       QString("form-data; name=\"%1\"; filename=\"%2\"")
                           .arg(fileFieldName).arg(QFileInfo(filePath).fileName()));
    filePart.setBodyDevice(file);
    file->setParent(multiPart); // 转移所有权
    multiPart->append(filePart);

    // 添加额外数据
    for (auto it = extraData.begin(); it != extraData.end(); ++it) {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, 
                           QString("form-data; name=\"%1\"").arg(it.key()));
        textPart.setBody(it.value().toVariant().toByteArray());
        multiPart->append(textPart);
    }

    // 创建请求（不设置 Content-Type，由 multipart 自动设置）
    QUrl url(m_baseUrl + path);
    QNetworkRequest request(url);
    if (!m_token.isEmpty()) {
        request.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());
    }

    // 发送请求
    QNetworkReply *reply = m_manager->post(request, multiPart);
    multiPart->setParent(reply); // 转移所有权

    // 连接上传进度
    if (onProgress) {
        connect(reply, &QNetworkReply::uploadProgress, this, [onProgress](qint64 bytesSent, qint64 bytesTotal) {
            onProgress(QJsonObject(), bytesSent, bytesTotal);
        });
    }

    // 连接完成信号
    connect(reply, &QNetworkReply::finished, this, [this, reply, onSuccess, onError]() {
        handleReply(reply, onSuccess, onError);
    });

    return reply;
}

void NetworkClient::handleReply(QNetworkReply *reply, SuccessHandler onSuccess, ErrorHandler onError, 
                                 bool isArray, ArraySuccessHandler onArraySuccess)
{
    reply->deleteLater();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        Logger::instance()->debug(QString("Response: %1").arg(QString::fromUtf8(data)), "network");

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull()) {
            Logger::instance()->error("Failed to parse JSON response", "network");
            if (onError) onError("服务器响应格式错误");
            return;
        }

        if (isArray) {
            if (onArraySuccess) onArraySuccess(doc.array());
        } else {
            if (onSuccess) onSuccess(doc.object());
        }
    } else {
        QString errorMsg = reply->errorString();
        Logger::instance()->warning(QString("Network Error [%1]: %2").arg(reply->url().toString()).arg(errorMsg), "network");

        if (onError) onError(errorMsg);
    }
}

void NetworkClient::retryRequest(const QString &path, const QJsonObject &data, const QString &method,
                                  SuccessHandler onSuccess, ErrorHandler onError, int attempt)
{
    // 重试逻辑已在 handleReply 中部分实现
    // 完整实现需要保存请求参数并在重试时重新发起
    Q_UNUSED(path)
    Q_UNUSED(data)
    Q_UNUSED(method)
    Q_UNUSED(onSuccess)
    Q_UNUSED(onError)
    Q_UNUSED(attempt)
}
