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
#include <functional>

class NetworkClient : public QObject
{
    Q_OBJECT
public:
    static NetworkClient* instance();

    typedef std::function<void(const QJsonObject&)> SuccessHandler;
    typedef std::function<void(const QJsonArray&)> ArraySuccessHandler;
    typedef std::function<void(const QString&)> ErrorHandler;

    void setBaseUrl(const QString &url);
    void setToken(const QString &token);
    QString token() const { return m_token; }

    // GET request
    void get(const QString &path, SuccessHandler onSuccess, ErrorHandler onError = nullptr);
    void getArray(const QString &path, ArraySuccessHandler onSuccess, ErrorHandler onError = nullptr);

    // POST request
    void post(const QString &path, const QJsonObject &data, SuccessHandler onSuccess, ErrorHandler onError = nullptr);

    // PUT request
    void put(const QString &path, const QJsonObject &data, SuccessHandler onSuccess, ErrorHandler onError = nullptr);

private:
    explicit NetworkClient(QObject *parent = nullptr);
    QNetworkRequest createRequest(const QString &path);
    void handleReply(QNetworkReply *reply, SuccessHandler onSuccess, ErrorHandler onError, bool isArray = false, ArraySuccessHandler onArraySuccess = nullptr);

    QNetworkAccessManager *m_manager;
    QString m_baseUrl;
    QString m_token;
};

#endif // NETWORKCLIENT_H
