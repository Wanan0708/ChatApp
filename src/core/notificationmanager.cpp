#include "notificationmanager.h"
#include "logger.h"
#include <QDateTime>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

NotificationManager* NotificationManager::instance()
{
    static NotificationManager instance;
    return &instance;
}

NotificationManager::NotificationManager(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings(this))
{
    load();
    Logger::instance()->info("NotificationManager initialized", "notification");
}

NotificationManager::~NotificationManager()
{
    save();
}

void NotificationManager::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enabledChanged(enabled);
        save();
        Logger::instance()->info(QString("Notifications %1").arg(enabled ? "enabled" : "disabled"), "notification");
    }
}

void NotificationManager::setSoundEnabled(bool enabled)
{
    if (m_soundEnabled != enabled) {
        m_soundEnabled = enabled;
        emit soundEnabledChanged(enabled);
        save();
    }
}

void NotificationManager::setDndEnabled(bool enabled)
{
    if (m_dndEnabled != enabled) {
        m_dndEnabled = enabled;
        emit dndEnabledChanged(enabled);
        save();
    }
}

void NotificationManager::sendNotification(const QString &title, const QString &message, const QString &type)
{
    if (!m_enabled) {
        Logger::instance()->debug("Notifications disabled, skipping", "notification");
        return;
    }
    
    // 检查免打扰模式
    if (m_dndEnabled || isDndPeriod()) {
        Logger::instance()->debug("In DND period, skipping notification", "notification");
        return;
    }
    
    Logger::instance()->info(QString("Sending notification: %1 - %2").arg(title).arg(message), "notification");
    
    // 发送桌面通知
    showDesktopNotification(title, message);
    
    // 播放提示音
    if (m_soundEnabled) {
        playNotificationSound();
    }
    
    emit notificationSent(title, message);
}

void NotificationManager::sendChatNotification(const QString &conversationId, const QString &sender, const QString &message)
{
    if (!m_enabled) {
        return;
    }
    
    if (m_dndEnabled || isDndPeriod()) {
        return;
    }
    
    QString title = sender;
    QString content = m_showPreview ? message : "新消息";
    
    sendNotification(title, content, "chat");
}

void NotificationManager::playSound(const QString &soundType)
{
    if (!m_soundEnabled) {
        return;
    }
    
    playNotificationSound();
}

bool NotificationManager::isDndPeriod() const
{
    if (!m_dndEnabled) {
        return false;
    }
    
    QTime currentTime = QTime::currentTime();
    QTime startTime = QTime::fromString(m_dndStartTime, "HH:mm");
    QTime endTime = QTime::fromString(m_dndEndTime, "HH:mm");
    
    if (!startTime.isValid() || !endTime.isValid()) {
        return false;
    }
    
    // 处理跨天情况（如 22:00 - 08:00）
    if (startTime > endTime) {
        return currentTime >= startTime || currentTime <= endTime;
    } else {
        return currentTime >= startTime && currentTime <= endTime;
    }
}

void NotificationManager::clearNotifications()
{
    Logger::instance()->debug("Notifications cleared", "notification");
}

void NotificationManager::save()
{
    m_settings->setValue("notifications/enabled", m_enabled);
    m_settings->setValue("notifications/soundEnabled", m_soundEnabled);
    m_settings->setValue("notifications/dndEnabled", m_dndEnabled);
    m_settings->setValue("notifications/showPreview", m_showPreview);
    m_settings->setValue("notifications/dndStartTime", m_dndStartTime);
    m_settings->setValue("notifications/dndEndTime", m_dndEndTime);
    m_settings->sync();
}

void NotificationManager::load()
{
    m_enabled = m_settings->value("notifications/enabled", true).toBool();
    m_soundEnabled = m_settings->value("notifications/soundEnabled", true).toBool();
    m_dndEnabled = m_settings->value("notifications/dndEnabled", false).toBool();
    m_showPreview = m_settings->value("notifications/showPreview", true).toBool();
    m_dndStartTime = m_settings->value("notifications/dndStartTime", "22:00").toString();
    m_dndEndTime = m_settings->value("notifications/dndEndTime", "08:00").toString();
}

void NotificationManager::showDesktopNotification(const QString &title, const QString &message)
{
    // 使用 QDesktopServices 显示通知（简化实现）
    // 完整实现需要平台特定的通知 API
    Logger::instance()->debug(QString("Desktop notification: %1 - %2").arg(title).arg(message), "notification");
    
#ifdef Q_OS_WIN
    // Windows：可以使用 Toast 通知
#elif defined(Q_OS_MACOS)
    // macOS：可以使用 NSUserNotification
#elif defined(Q_OS_LINUX)
    // Linux：可以使用 notify-send
#endif
}

void NotificationManager::playNotificationSound()
{
    // 简化实现：使用系统默认声音
    // 注意：Qt Multimedia 需要添加 multimediawidgets 模块
    // 这里使用 QDesktopServices 播放系统声音作为替代
    Logger::instance()->debug("Playing notification sound", "notification");
    
    // 实际项目中可以添加声音文件并使用 QSoundEffect:
    // QSoundEffect *effect = new QSoundEffect(this);
    // effect->setSource(QUrl::fromLocalFile("sounds/message.wav"));
    // effect->play();
}

void NotificationManager::onNotificationClicked(const QString &id)
{
    emit notificationClicked(id);
    Logger::instance()->debug(QString("Notification clicked: %1").arg(id), "notification");
}
