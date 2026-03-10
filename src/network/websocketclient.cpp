// Sources/network/websocketclient.cpp
#include "websocketclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDateTime>
#include <QRandomGenerator>

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent)
    , m_webSocket(new QWebSocket("ChatAppClient", QWebSocketProtocol::VersionLatest, this))
    , m_reconnectTimer(new QTimer(this))
    , m_heartbeatTimer(new QTimer(this))
    , m_pongTimer(new QTimer(this))
{
    // 连接 WebSocket 信号
    connect(m_webSocket, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessageReceived);
    connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, &WebSocketClient::onError);
    connect(m_webSocket, &QWebSocket::pong, this, &WebSocketClient::onPong);

    // 配置重连定时器
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &WebSocketClient::onReconnectTimer);

    // 配置心跳定时器
    m_heartbeatTimer->setInterval(m_heartbeatInterval);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &WebSocketClient::onHeartbeatTimer);

    m_pongTimer->setSingleShot(true);
    connect(m_pongTimer, &QTimer::timeout, [this]() {
        if (m_isConnected && m_awaitingPong) {
            qWarning() << "[WebSocketClient] Heartbeat timeout, reconnecting...";
            m_webSocket->close();
        }
    });

    qDebug() << "[WebSocketClient] Created";
}

WebSocketClient::~WebSocketClient()
{
    disconnectFromServer();
}

bool WebSocketClient::isReconnecting() const
{
    return !m_isConnected
        && (m_connectionState.startsWith(QStringLiteral("Connecting"))
            || m_connectionState.startsWith(QStringLiteral("Reconnecting")));
}

void WebSocketClient::connectToServer(const QString &url)
{
    const QString normalizedUrl = url.trimmed();
    if (normalizedUrl.isEmpty()) {
        emit errorOccurred("WebSocket URL is empty");
        return;
    }

    m_manualDisconnectRequested = false;
    m_reconnectTimer->stop();

    if (m_isConnected) {
        qDebug() << "[WebSocketClient] Already connected, disconnecting first";
        disconnectFromServer();
        m_manualDisconnectRequested = false;
    }

    if (m_webSocket->state() == QAbstractSocket::ConnectingState) {
        qDebug() << "[WebSocketClient] Already connecting, skipping";
        return;
    }

    m_serverUrl = normalizedUrl;
    m_connectionState = "Connecting...";
    emit connectionStateChanged(m_connectionState);

    qDebug() << "[WebSocketClient] Connecting to:" << normalizedUrl;
    m_webSocket->open(QUrl(normalizedUrl));
}

void WebSocketClient::disconnectFromServer()
{
    m_manualDisconnectRequested = true;
    m_heartbeatTimer->stop();
    m_pongTimer->stop();
    m_reconnectTimer->stop();

    if (m_webSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "[WebSocketClient] Disconnecting from server";
        m_webSocket->close();
    } else {
        m_connectionState = QStringLiteral("Disconnected");
        emit connectionStateChanged(m_connectionState);
    }

    resetReconnectState();
}

void WebSocketClient::sendMessage(const QVariantMap &message)
{
    if (!m_isConnected) {
        qDebug() << "[WebSocketClient] Not connected, queuing message";
        if (m_messageQueue.size() >= m_maxQueuedMessages) {
            m_messageQueue.takeFirst();
            qWarning() << "[WebSocketClient] Message queue full, dropped oldest message";
        }
        m_messageQueue.append(message);
        return;
    }

    QString jsonStr = serializeMessage(message);
    m_webSocket->sendTextMessage(jsonStr);
    qDebug() << "[WebSocketClient] Sent message:" << jsonStr.left(100);
}

void WebSocketClient::onConnected()
{
    m_isConnected = true;
    m_connectionState = "Connected";
    m_reconnectAttempts = 0;
    m_currentReconnectDelay = m_initialReconnectDelay;
    m_manualDisconnectRequested = false;

    emit connected();
    emit connectedChanged(true);
    emit connectionStateChanged(m_connectionState);

    // 启动心跳
    m_heartbeatTimer->start();
    sendHeartbeat();

    // 发送队列中的消息
    while (!m_messageQueue.isEmpty()) {
        sendMessage(m_messageQueue.takeFirst());
    }

    qDebug() << "[WebSocketClient] Connected successfully";
}

void WebSocketClient::onDisconnected()
{
    m_isConnected = false;
    m_connectionState = "Disconnected";
    m_awaitingPong = false;

    emit disconnected();
    emit connectedChanged(false);
    emit connectionStateChanged(m_connectionState);

    m_heartbeatTimer->stop();
    m_pongTimer->stop();

    // 自动重连（排除用户主动断开）
    const bool unlimitedReconnect = (m_maxReconnectAttempts <= 0);
    if (!m_manualDisconnectRequested && !m_serverUrl.isEmpty()
        && (unlimitedReconnect || m_reconnectAttempts < m_maxReconnectAttempts)) {
        attemptReconnect();
    } else if (!m_manualDisconnectRequested && !m_serverUrl.isEmpty() && !unlimitedReconnect
               && m_reconnectAttempts >= m_maxReconnectAttempts) {
        emit reconnectFailed();
    }

    qDebug() << "[WebSocketClient] Disconnected";
}

void WebSocketClient::onTextMessageReceived(const QString &message)
{
    // 忽略心跳响应
    if (message == "{\"type\":\"heartbeat\"}") {
        return;
    }

    QVariantMap parsed = parseMessage(message);
    if (!parsed.isEmpty()) {
        // 处理服务器心跳响应
        if (parsed.value("type") == "heartbeat") {
            m_awaitingPong = false;
            m_pongTimer->stop();
            qDebug() << "[WebSocketClient] Heartbeat acknowledged";
            return;
        }

        // 打印所有接收到的消息类型，便于调试
        qDebug() << "[WebSocketClient] Received message type:" << parsed.value("type").toString()
                 << "content:" << message.left(200);

        emit messageReceived(parsed);
    }
}

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    QString errorMsg = m_webSocket->errorString();
    m_connectionState = "Error: " + errorMsg;
    emit connectionStateChanged(m_connectionState);
    emit errorOccurred(errorMsg);

    qDebug() << "[WebSocketClient] Error:" << error << errorMsg;
}

void WebSocketClient::onReconnectTimer()
{
    if (m_maxReconnectAttempts > 0 && m_reconnectAttempts >= m_maxReconnectAttempts) {
        qWarning() << "[WebSocketClient] Max reconnect attempts reached, giving up";
        return;
    }

    qDebug() << "[WebSocketClient] Attempting reconnect (attempt" << m_reconnectAttempts << ")";
    m_webSocket->open(QUrl(m_serverUrl));
}

void WebSocketClient::onHeartbeatTimer()
{
    if (m_isConnected) {
        sendHeartbeat();
    }
}

void WebSocketClient::onPong(quint64 elapsedTime)
{
    m_awaitingPong = false;
    m_pongTimer->stop();
    qDebug() << "[WebSocketClient] Pong received in" << elapsedTime << "ms - Connection healthy";
}

void WebSocketClient::attemptReconnect()
{
    m_reconnectAttempts++;
    if (m_maxReconnectAttempts > 0) {
        m_connectionState = QString("Reconnecting... (attempt %1/%2)")
            .arg(m_reconnectAttempts)
            .arg(m_maxReconnectAttempts);
    } else {
        m_connectionState = QString("Reconnecting... (attempt %1)")
            .arg(m_reconnectAttempts);
    }
    emit connectionStateChanged(m_connectionState);

    // 在当前延迟基础上增加抖动，避免多个客户端同时重连
    const int jitter = QRandomGenerator::global()->bounded(201) - 100; // [-100, 100] ms
    const int delayWithJitter = qMax(200, m_currentReconnectDelay + jitter);
    m_reconnectTimer->start(delayWithJitter);
    qDebug() << "[WebSocketClient] Scheduling reconnect in" << delayWithJitter << "ms";

    // 为下一次重连准备更大的延迟（指数退避，最大60秒）
    m_currentReconnectDelay = qMin(m_currentReconnectDelay * 2, 60000);
}

void WebSocketClient::sendHeartbeat()
{
    QVariantMap heartbeat;
    heartbeat["type"] = "heartbeat";
    heartbeat["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    m_webSocket->sendTextMessage(serializeMessage(heartbeat));

    m_awaitingPong = true;
    m_pongTimer->start(m_heartbeatTimeout);

    qDebug() << "[WebSocketClient] Sent heartbeat, awaiting response within" << m_heartbeatTimeout << "ms";
}

void WebSocketClient::resetReconnectState()
{
    m_reconnectAttempts = 0;
    m_currentReconnectDelay = m_initialReconnectDelay;
    m_messageQueue.clear();
    m_awaitingPong = false;
}

void WebSocketClient::setHeartbeatInterval(int ms)
{
    m_heartbeatInterval = qMax(1000, ms);
    m_heartbeatTimer->setInterval(m_heartbeatInterval);
}

void WebSocketClient::setReconnectDelay(int initialDelayMs)
{
    m_initialReconnectDelay = qMax(200, initialDelayMs);
    m_currentReconnectDelay = m_initialReconnectDelay;
}

QVariantMap WebSocketClient::parseMessage(const QString &jsonStr) const
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "[WebSocketClient] JSON parse error:" << error.errorString();
        return QVariantMap();
    }

    if (!doc.isObject()) {
        qWarning() << "[WebSocketClient] JSON is not an object";
        return QVariantMap();
    }

    return doc.object().toVariantMap();
}

QString WebSocketClient::serializeMessage(const QVariantMap &message) const
{
    QJsonObject obj = QJsonObject::fromVariantMap(message);
    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}
