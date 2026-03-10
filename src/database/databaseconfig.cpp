#include "databaseconfig.h"
#include <QProcessEnvironment>
#include <QDebug>
#include <QUrl>

QString DatabaseConfig::defaultHost()
{
#ifdef QT_DEBUG
    return "127.0.0.1";
#else
    return "59.110.44.145";
#endif
}

QString DatabaseConfig::backendHost()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    return env.value("CHATAPP_BACKEND_HOST", defaultHost());
}

int DatabaseConfig::backendPort()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    bool ok = false;
    const int port = env.value("CHATAPP_BACKEND_PORT", "8080").toInt(&ok);
    return (ok && port > 0 && port <= 65535) ? port : 8080;
}

QString DatabaseConfig::backendBaseUrl()
{
    QUrl url(apiUrl());
    if (url.isValid()) {
        url.setPath(QString());
        url.setQuery(QString());
        url.setFragment(QString());
        QString base = url.toString(QUrl::FullyEncoded);
        if (base.endsWith('/')) {
            base.chop(1);
        }
        return base;
    }

    const QString scheme = QProcessEnvironment::systemEnvironment().value("CHATAPP_API_SCHEME", "http");
    return QString("%1://%2:%3").arg(scheme, backendHost()).arg(backendPort());
}

QString DatabaseConfig::apiUrl()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString baseUrl = env.value("CHATAPP_API_BASE_URL").trimmed();

    if (!baseUrl.isEmpty()) {
        QUrl url(baseUrl);
        if (url.isValid()) {
#ifndef QT_DEBUG
            if (url.scheme().toLower() == "http") {
                qWarning() << "[DatabaseConfig] Insecure API scheme in RELEASE mode:" << url.toString();
            }
#endif
            const QString path = url.path();
            if (path.isEmpty() || path == "/") {
                url.setPath("/api");
            }
            return url.toString(QUrl::FullyEncoded);
        }
        // 回退：非标准 URL 仅做最小处理
        if (baseUrl.endsWith('/')) {
            baseUrl.chop(1);
        }
        if (!baseUrl.endsWith("/api")) {
            baseUrl += "/api";
        }
        return baseUrl;
    }

    const QString scheme = env.value("CHATAPP_API_SCHEME", "http");
#ifndef QT_DEBUG
    if (scheme.toLower() == "http") {
        qWarning() << "[DatabaseConfig] Using insecure API scheme in RELEASE mode. Consider CHATAPP_API_SCHEME=https.";
    }
#endif
    return QString("%1://%2:%3/api").arg(scheme, backendHost()).arg(backendPort());
}

QString DatabaseConfig::wsUrl()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString overrideUrl = env.value("CHATAPP_WS_URL").trimmed();
    if (!overrideUrl.isEmpty()) {
#ifndef QT_DEBUG
        const QUrl wsOverride(overrideUrl);
        if (wsOverride.isValid() && wsOverride.scheme().toLower() == "ws") {
            qWarning() << "[DatabaseConfig] Insecure WebSocket scheme in RELEASE mode:" << overrideUrl;
        }
#endif
        return overrideUrl;
    }

    const QString scheme = env.value("CHATAPP_WS_SCHEME", "ws");
#ifndef QT_DEBUG
    if (scheme.toLower() == "ws") {
        qWarning() << "[DatabaseConfig] Using insecure WebSocket scheme in RELEASE mode. Consider CHATAPP_WS_SCHEME=wss.";
    }
#endif
    return QString("%1://%2:%3").arg(scheme, backendHost()).arg(backendPort());
}

QString DatabaseConfig::resolveServerUrl(const QString &pathOrUrl)
{
    const QString trimmed = pathOrUrl.trimmed();
    if (trimmed.isEmpty()) {
        return trimmed;
    }

    const QString lower = trimmed.toLower();
    if (lower.startsWith("http://") || lower.startsWith("https://")
        || lower.startsWith("file://") || lower.startsWith("qrc:")) {
        return trimmed;
    }

    if (trimmed.startsWith('/')) {
        return backendBaseUrl() + trimmed;
    }

    return backendBaseUrl() + "/" + trimmed;
}

QString DatabaseConfig::toServerRelativePath(const QString &pathOrUrl)
{
    const QString trimmed = pathOrUrl.trimmed();
    if (trimmed.isEmpty()) {
        return trimmed;
    }

    if (trimmed.startsWith('/')) {
        return trimmed;
    }

    QUrl url(trimmed);
    if (!url.isValid() || url.scheme().isEmpty()) {
        return trimmed;
    }

    const QString path = url.path();
    return path.isEmpty() ? trimmed : path;
}

DatabaseConfig DatabaseConfig::loadFromEnvironment()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    
    DatabaseConfig config;
#ifdef QT_DEBUG
    config.host = env.value("CHATAPP_DB_HOST", "localhost");
#else
    config.host = env.value("CHATAPP_DB_HOST", "59.110.44.145");
#endif
    config.database = env.value("CHATAPP_DB_NAME", "chatapp");
    config.username = env.value("CHATAPP_DB_USER", "postgres");
    config.password = env.value("CHATAPP_DB_PASSWORD", "123456");
    config.port = env.value("CHATAPP_DB_PORT", "5432").toInt();

    qDebug() << "[DatabaseConfig] Loaded from environment:"
             << "Host:" << config.host
             << "Database:" << config.database
             << "User:" << config.username
             << "Port:" << config.port
             << "API URL:" << apiUrl()
             << "WS URL:" << wsUrl();

    return config;
}

bool DatabaseConfig::isValid() const
{
    return !host.isEmpty() && !database.isEmpty() && !username.isEmpty() && port > 0;
}
