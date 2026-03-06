// Headers/network/websocketclient.h
#pragma once

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QVariantMap>

/**
 * @brief WebSocket 客户端封装（生产级实现）
 *
 * 特性：
 * - 自动重连（指数退避）
 * - 心跳保活（30秒）
 * - 消息队列（断线缓存）
 * - 连接状态管理
 * - 完整错误处理
 */
class WebSocketClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QString connectionState READ connectionState NOTIFY connectionStateChanged)

public:
    explicit WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient() override;

    // 连接管理
    Q_INVOKABLE void connectToServer(const QString &url);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void sendMessage(const QVariantMap &message);

    // 状态查询
    bool isConnected() const { return m_isConnected; }
    QString connectionState() const { return m_connectionState; }

    // 重连配置
    void setHeartbeatInterval(int ms);
    void setMaxReconnectAttempts(int attempts) { m_maxReconnectAttempts = attempts; }
    void setReconnectDelay(int initialDelayMs);

signals:
    void connected();
    void disconnected();
    void connectedChanged(bool connected);
    void connectionStateChanged(const QString &state);
    void messageReceived(const QVariantMap &message);
    void errorOccurred(const QString &error);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);
    void onReconnectTimer();
    void onHeartbeatTimer();
    void onPong(quint64 elapsedTime);

private:
    void attemptReconnect();
    void sendHeartbeat();
    void resetReconnectState();
    QVariantMap parseMessage(const QString &jsonStr) const;
    QString serializeMessage(const QVariantMap &message) const;

    QWebSocket *m_webSocket;
    QTimer *m_reconnectTimer;
    QTimer *m_heartbeatTimer;

    QString m_serverUrl;
    bool m_isConnected = false;
    QString m_connectionState = "Disconnected";

    // 重连状态
    int m_reconnectAttempts = 0;
    int m_maxReconnectAttempts = 10;
    int m_initialReconnectDelay = 1000; // 1秒
    int m_currentReconnectDelay = 1000;

    // 心跳配置
    int m_heartbeatInterval = 30000;  // 30秒
    int m_heartbeatTimeout = 5000;    // 5秒超时
    bool m_awaitingPong = false;
    QTimer *m_pongTimer;

    // 消息队列（断线时缓存）
    QList<QVariantMap> m_messageQueue;
    bool m_manualDisconnectRequested = false;
    int m_maxQueuedMessages = 200;
};
