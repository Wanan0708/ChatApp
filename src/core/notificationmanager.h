#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QSettings>

/**
 * @brief 通知管理器
 * 
 * 提供应用通知管理功能
 * 
 * 特性：
 * - 桌面通知
 * - 声音提醒
 * - 通知设置
 * - 免打扰模式
 * - 通知历史
 */
class NotificationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)
    Q_PROPERTY(bool dndEnabled READ dndEnabled WRITE setDndEnabled NOTIFY dndEnabledChanged)
    
public:
    static NotificationManager* instance();

    // 通知开关
    Q_INVOKABLE bool enabled() const { return m_enabled; }
    Q_INVOKABLE void setEnabled(bool enabled);
    
    Q_INVOKABLE bool soundEnabled() const { return m_soundEnabled; }
    Q_INVOKABLE void setSoundEnabled(bool enabled);
    
    Q_INVOKABLE bool dndEnabled() const { return m_dndEnabled; }
    Q_INVOKABLE void setDndEnabled(bool enabled);
    
    // 发送通知
    Q_INVOKABLE void sendNotification(const QString &title, const QString &message, const QString &type = "info");
    Q_INVOKABLE void sendChatNotification(const QString &conversationId, const QString &sender, const QString &message);
    
    // 播放声音
    Q_INVOKABLE void playSound(const QString &soundType = "message");
    
    // 通知设置
    Q_INVOKABLE void setShowPreview(bool show) { m_showPreview = show; save(); }
    Q_INVOKABLE bool showPreview() const { return m_showPreview; }
    
    // 免打扰时段
    Q_INVOKABLE void setDndStartTime(const QString &time) { m_dndStartTime = time; save(); }
    Q_INVOKABLE void setDndEndTime(const QString &time) { m_dndEndTime = time; save(); }
    Q_INVOKABLE QString dndStartTime() const { return m_dndStartTime; }
    Q_INVOKABLE QString dndEndTime() const { return m_dndEndTime; }
    
    // 检查是否在免打扰时段
    Q_INVOKABLE bool isDndPeriod() const;
    
    // 清除通知
    Q_INVOKABLE void clearNotifications();
    
    // 保存/加载设置
    Q_INVOKABLE void save();
    Q_INVOKABLE void load();

signals:
    void enabledChanged(bool enabled);
    void soundEnabledChanged(bool enabled);
    void dndEnabledChanged(bool enabled);
    void notificationSent(const QString &title, const QString &message);
    void notificationClicked(const QString &id);

private slots:
    void onNotificationClicked(const QString &id);

private:
    explicit NotificationManager(QObject *parent = nullptr);
    ~NotificationManager() override;

    void showDesktopNotification(const QString &title, const QString &message);
    void playNotificationSound();

    bool m_enabled = true;
    bool m_soundEnabled = true;
    bool m_dndEnabled = false;
    bool m_showPreview = true;
    
    QString m_dndStartTime = "22:00";
    QString m_dndEndTime = "08:00";
    
    QSettings *m_settings;
};

#endif // NOTIFICATIONMANAGER_H
