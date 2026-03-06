#include "networkclient.h"
#include "../database/databaseconfig.h"
#include <QDebug>

NetworkClient* NetworkClient::instance()
{
    static NetworkClient *inst = new NetworkClient();
    return inst;
}

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_baseUrl(DatabaseConfig::apiUrl()) // Use DatabaseConfig::apiUrl()
{
    qDebug() << "[NetworkClient] Initialized with Base URL:" << m_baseUrl;
}

void NetworkClient::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
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
        qDebug() << "[NetworkClient] Adding Authorization header, token length:" << m_token.length();
        request.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());
    } else {
        qDebug() << "[NetworkClient] No token, skipping Authorization header";
    }

    return request;
}

void NetworkClient::get(const QString &path, SuccessHandler onSuccess, ErrorHandler onError)
{
    QNetworkReply *reply = m_manager->get(createRequest(path));
    connect(reply, &QNetworkReply::finished, [this, reply, onSuccess, onError]() {
        handleReply(reply, onSuccess, onError);
    });
}

void NetworkClient::getArray(const QString &path, ArraySuccessHandler onSuccess, ErrorHandler onError)
{
    QNetworkReply *reply = m_manager->get(createRequest(path));
    connect(reply, &QNetworkReply::finished, [this, reply, onSuccess, onError]() {
        handleReply(reply, nullptr, onError, true, onSuccess);
    });
}

void NetworkClient::post(const QString &path, const QJsonObject &data, SuccessHandler onSuccess, ErrorHandler onError)
{
    QNetworkRequest request = createRequest(path);
    qDebug() << "[NetworkClient] POST request to:" << request.url().toString();
    qDebug() << "[NetworkClient] Request data:" << QJsonDocument(data).toJson();
    
    QNetworkReply *reply = m_manager->post(request, QJsonDocument(data).toJson());
    qDebug() << "[NetworkClient] Reply created:" << (reply != nullptr);
    
    connect(reply, &QNetworkReply::finished, [this, reply, onSuccess, onError]() {
        qDebug() << "[NetworkClient] Reply finished, error:" << reply->error();
        handleReply(reply, onSuccess, onError);
    });
}

void NetworkClient::put(const QString &path, const QJsonObject &data, SuccessHandler onSuccess, ErrorHandler onError)
{
    QNetworkReply *reply = m_manager->put(createRequest(path), QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, [this, reply, onSuccess, onError]() {
        handleReply(reply, onSuccess, onError);
    });
}

void NetworkClient::handleReply(QNetworkReply *reply, SuccessHandler onSuccess, ErrorHandler onError, bool isArray, ArraySuccessHandler onArraySuccess)
{
    reply->deleteLater();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        qDebug() << "[NetworkClient] Response data:" << data;
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull()) {
            qWarning() << "[NetworkClient] Failed to parse JSON response";
            if (onError) onError("Invalid JSON response from server");
            return;
        }

        if (isArray) {
            if (onArraySuccess) onArraySuccess(doc.array());
        } else {
            qDebug() << "[NetworkClient] Calling success handler";
            if (onSuccess) onSuccess(doc.object());
        }
    } else {
        QString errorMsg = reply->errorString();
        qWarning() << "[NetworkClient] Network Error [" << reply->url().toString() << "]:" << errorMsg;
        if (onError) onError(errorMsg);
    }
}
