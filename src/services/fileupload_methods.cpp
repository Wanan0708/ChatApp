// Sources/services/fileupload_methods.cpp
// 文件上传方法实现

#include "chatservice.h"
#include "../models/conversationmodel.h"
#include "../models/messagemodel.h"
#include "../network/networkclient.h"
#include "../network/websocketclient.h"
#include "../utils/messagecache.h"
#include "../utils/messagepreview.h"
#include "../utils/timeformatter.h"
#include <QDateTime>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QUrl>

namespace {
QString toLocalPreviewUrl(const QString &filePath)
{
    return QUrl::fromLocalFile(QFileInfo(filePath).absoluteFilePath()).toString();
}
}

QString ChatService::pickLocalFile(bool imageOnly) const
{
    QString filter = imageOnly
        ? QStringLiteral("Image files (*.png *.jpg *.jpeg *.gif *.bmp *.webp)")
        : QStringLiteral("All files (*)");

    return QFileDialog::getOpenFileName(nullptr,
                                        imageOnly ? QStringLiteral("选择图片发送")
                                                  : QStringLiteral("选择文件发送"),
                                        QString(),
                                        filter);
}

void ChatService::sendImageMessage(const QString &conversationId, const QString &filePath)
{
    sendImageMessageInternal(conversationId, filePath);
}

void ChatService::sendImageMessageInternal(const QString &conversationId,
                                          const QString &filePath,
                                          const QString &messageId,
                                          qint64 timestamp,
                                          bool updateExisting)
{
    if (filePath.isEmpty()) return;

    maybeRestoreConversationVisibility(conversationId);

    qDebug() << "[ChatService] Sending image message:" << filePath;
    const QString localPreviewUrl = toLocalPreviewUrl(filePath);
    const QString localMessageId = messageId.isEmpty()
        ? QString("local-image-%1-%2")
              .arg(QDateTime::currentMSecsSinceEpoch())
              .arg(QRandomGenerator::global()->generate())
        : messageId;
    const qint64 currentTimestamp = timestamp > 0 ? timestamp : QDateTime::currentMSecsSinceEpoch();

    QVariantMap tempMessage;
    tempMessage["messageId"] = localMessageId;
    tempMessage["type"] = 1;  // 图片类型
    tempMessage["conversationId"] = conversationId;
    tempMessage["content"] = "[图片]";
    tempMessage["senderId"] = m_currentUserId;
    tempMessage["senderName"] = m_currentUserName;
    tempMessage["timestamp"] = currentTimestamp;
    tempMessage["status"] = 0;  // 发送中
    tempMessage["fileName"] = QFileInfo(filePath).fileName();
    tempMessage["fileUrl"] = localPreviewUrl;
    tempMessage["thumbnailUrl"] = localPreviewUrl;
    tempMessage["isOffline"] = false;
    tempMessage["serverMessageId"] = QString();

    getMessageModel(conversationId)->upsertMessage(tempMessage);
    MessageCache::instance()->cacheMessage(conversationId, tempMessage);
    m_conversationModel->updateConversation(conversationId, {
        {"lastMessage", MessagePreview::normalizeConversationPreview(tempMessage["content"].toString(), 1)},
        {"time", TimeFormatter::formatChatTime(currentTimestamp)},
        {"unreadCount", 0}
    });

    QVariantMap retryInfo;
    retryInfo["conversationId"] = conversationId;
    retryInfo["messageType"] = 1;
    retryInfo["localFilePath"] = filePath;
    retryInfo["timestamp"] = currentTimestamp;
    retryInfo["content"] = QStringLiteral("[图片]");
    retryInfo["fileName"] = QFileInfo(filePath).fileName();
    retryInfo["fileUrl"] = localPreviewUrl;
    retryInfo["thumbnailUrl"] = localPreviewUrl;
    rememberRetryableMessage(localMessageId, retryInfo);

    NetworkClient::UploadProgressHandler progressHandler = [](const QJsonObject &, qint64, qint64) {
    };

    QString fp = filePath;
    NetworkClient::SuccessHandler successHandler = [this, conversationId, fp, localMessageId, currentTimestamp](const QJsonObject &result) {
        QString fileId = result.value("fileId").toString();
        QString fileUrl = result.value("fileUrl").toString();
        QString thumbnailUrl = result.value("thumbnailUrl").toString(fileUrl);
        const bool queuedBeforeDispatch = !m_webSocketClient->isConnected();

        QVariantMap message;
        message["messageId"] = localMessageId;
        message["type"] = 1;  // 图片类型
        message["conversationId"] = conversationId;
        message["content"] = "[图片]";
        message["senderId"] = m_currentUserId;
        message["senderName"] = m_currentUserName;
        message["timestamp"] = currentTimestamp;
        message["status"] = static_cast<int>(queuedBeforeDispatch ? MessageStatus::Sending : MessageStatus::Sent);
        message["fileId"] = fileId;
        message["fileName"] = QFileInfo(fp).fileName();
        message["fileUrl"] = fileUrl;
        message["thumbnailUrl"] = thumbnailUrl;
        message["isOffline"] = queuedBeforeDispatch;
        message["errorText"] = QString();
        message["serverMessageId"] = QString();

        getMessageModel(conversationId)->upsertMessage(message);
        MessageCache::instance()->cacheMessage(conversationId, message);

        QVariantMap wsMessage;
        wsMessage["type"] = "message";
        wsMessage["messageType"] = 1;
        wsMessage["conversationId"] = conversationId;
        wsMessage["content"] = "[图片]";
        wsMessage["senderId"] = m_currentUserId;
        wsMessage["senderName"] = m_currentUserName;
        wsMessage["timestamp"] = currentTimestamp;
        wsMessage["status"] = 1;
        wsMessage["fileId"] = fileId;
        wsMessage["fileName"] = QFileInfo(fp).fileName();
        wsMessage["fileUrl"] = fileUrl;
        wsMessage["thumbnailUrl"] = thumbnailUrl;

        QVariantMap retryInfo = m_retryableMessages.value(localMessageId);
        retryInfo["fileId"] = fileId;
        retryInfo["fileUrl"] = fileUrl;
        retryInfo["thumbnailUrl"] = thumbnailUrl;
        retryInfo["content"] = "[图片]";
        retryInfo["fileName"] = QFileInfo(fp).fileName();
        rememberRetryableMessage(localMessageId, retryInfo);

        dispatchOutgoingMessage(conversationId, localMessageId, wsMessage, queuedBeforeDispatch);

        emit messageReceived(conversationId, message);
    };

    NetworkClient::ErrorHandler errorHandler = [this, conversationId, fp, localMessageId, currentTimestamp, localPreviewUrl](const QString &error) {
        qWarning() << "[ChatService] Image upload failed:" << error;

        QVariantMap failedMessage;
        failedMessage["messageId"] = localMessageId;
        failedMessage["type"] = 1;
        failedMessage["conversationId"] = conversationId;
        failedMessage["content"] = "[图片发送失败]";
        failedMessage["senderId"] = m_currentUserId;
        failedMessage["senderName"] = m_currentUserName;
        failedMessage["timestamp"] = currentTimestamp;
        failedMessage["status"] = 3;  // 失败
        failedMessage["fileName"] = QFileInfo(fp).fileName();
        failedMessage["fileUrl"] = localPreviewUrl;
        failedMessage["thumbnailUrl"] = localPreviewUrl;
        failedMessage["isOffline"] = true;
        failedMessage["errorText"] = error.isEmpty() ? QStringLiteral("图片上传失败，点击重试") : error;
        failedMessage["serverMessageId"] = QString();

        getMessageModel(conversationId)->upsertMessage(failedMessage);
        MessageCache::instance()->cacheMessage(conversationId, failedMessage);

        emit messageReceived(conversationId, failedMessage);
    };

    NetworkClient::instance()->uploadFile(
        "/upload/image",
        filePath,
        "image",
        QJsonObject(),
        progressHandler,
        successHandler,
        errorHandler
    );
}

void ChatService::sendFileMessage(const QString &conversationId, const QString &filePath)
{
    sendFileMessageInternal(conversationId, filePath);
}

void ChatService::sendFileMessageInternal(const QString &conversationId,
                                         const QString &filePath,
                                         const QString &messageId,
                                         qint64 timestamp,
                                         bool updateExisting)
{
    if (filePath.isEmpty()) return;

    maybeRestoreConversationVisibility(conversationId);

    qDebug() << "[ChatService] Sending file message:" << filePath;
    const QString localMessageId = messageId.isEmpty()
        ? QString("local-file-%1-%2")
              .arg(QDateTime::currentMSecsSinceEpoch())
              .arg(QRandomGenerator::global()->generate())
        : messageId;
    const qint64 currentTimestamp = timestamp > 0 ? timestamp : QDateTime::currentMSecsSinceEpoch();

    QVariantMap tempMessage;
    tempMessage["messageId"] = localMessageId;
    tempMessage["type"] = 2;  // 文件类型
    tempMessage["conversationId"] = conversationId;
    tempMessage["content"] = "[文件]";
    tempMessage["senderId"] = m_currentUserId;
    tempMessage["senderName"] = m_currentUserName;
    tempMessage["timestamp"] = currentTimestamp;
    tempMessage["status"] = 0;  // 发送中
    tempMessage["fileName"] = QFileInfo(filePath).fileName();
    tempMessage["isOffline"] = false;
    tempMessage["serverMessageId"] = QString();

    getMessageModel(conversationId)->upsertMessage(tempMessage);
    MessageCache::instance()->cacheMessage(conversationId, tempMessage);
    m_conversationModel->updateConversation(conversationId, {
        {"lastMessage", MessagePreview::normalizeConversationPreview(tempMessage["content"].toString(), 2)},
        {"time", TimeFormatter::formatChatTime(currentTimestamp)},
        {"unreadCount", 0}
    });

    QVariantMap retryInfo;
    retryInfo["conversationId"] = conversationId;
    retryInfo["messageType"] = 2;
    retryInfo["localFilePath"] = filePath;
    retryInfo["timestamp"] = currentTimestamp;
    retryInfo["content"] = QStringLiteral("[文件]");
    retryInfo["fileName"] = QFileInfo(filePath).fileName();
    rememberRetryableMessage(localMessageId, retryInfo);

    NetworkClient::UploadProgressHandler progressHandler = [](const QJsonObject &, qint64, qint64) {
    };

    QString fp = filePath;
    NetworkClient::SuccessHandler successHandler = [this, conversationId, fp, localMessageId, currentTimestamp](const QJsonObject &result) {
        QString fileId = result.value("fileId").toString();
        QString fileUrl = result.value("fileUrl").toString();
        qint64 fileSize = result.value("fileSize").toVariant().toLongLong();
        const bool queuedBeforeDispatch = !m_webSocketClient->isConnected();

        // 格式化文件大小
        QString sizeStr;
        if (fileSize < 1024) {
            sizeStr = QString::number(fileSize) + " B";
        } else if (fileSize < 1024 * 1024) {
            sizeStr = QString::number(fileSize / 1024.0, 'f', 1) + " KB";
        } else if (fileSize < 1024 * 1024 * 1024) {
            sizeStr = QString::number(fileSize / (1024.0 * 1024.0), 'f', 1) + " MB";
        } else {
            sizeStr = QString::number(fileSize / (1024.0 * 1024.0 * 1024.0), 'f', 1) + " GB";
        }

        // 将临时消息更新为已发送状态
        QVariantMap message;
        message["messageId"] = localMessageId;
        message["type"] = 2;  // 文件类型
        message["conversationId"] = conversationId;
        message["content"] = "[文件]";
        message["senderId"] = m_currentUserId;
        message["senderName"] = m_currentUserName;
        message["timestamp"] = currentTimestamp;
        message["status"] = static_cast<int>(queuedBeforeDispatch ? MessageStatus::Sending : MessageStatus::Sent);
        message["fileId"] = fileId;
        message["fileName"] = QFileInfo(fp).fileName();
        message["fileSize"] = sizeStr;
        message["fileUrl"] = fileUrl;
        message["isOffline"] = queuedBeforeDispatch;
        message["errorText"] = QString();
        message["serverMessageId"] = QString();

        getMessageModel(conversationId)->upsertMessage(message);
        MessageCache::instance()->cacheMessage(conversationId, message);

        QVariantMap wsMessage;
        wsMessage["type"] = "message";
        wsMessage["messageType"] = 2;
        wsMessage["conversationId"] = conversationId;
        wsMessage["content"] = "[文件]";
        wsMessage["senderId"] = m_currentUserId;
        wsMessage["senderName"] = m_currentUserName;
        wsMessage["timestamp"] = currentTimestamp;
        wsMessage["status"] = 1;
        wsMessage["fileId"] = fileId;
        wsMessage["fileName"] = QFileInfo(fp).fileName();
        wsMessage["fileSize"] = sizeStr;
        wsMessage["fileUrl"] = fileUrl;

        QVariantMap retryInfo = m_retryableMessages.value(localMessageId);
        retryInfo["fileId"] = fileId;
        retryInfo["fileUrl"] = fileUrl;
        retryInfo["fileSize"] = sizeStr;
        retryInfo["content"] = "[文件]";
        retryInfo["fileName"] = QFileInfo(fp).fileName();
        rememberRetryableMessage(localMessageId, retryInfo);

        dispatchOutgoingMessage(conversationId, localMessageId, wsMessage, queuedBeforeDispatch);

        emit messageReceived(conversationId, message);
    };

    NetworkClient::ErrorHandler errorHandler = [this, conversationId, fp, localMessageId, currentTimestamp](const QString &error) {
        qWarning() << "[ChatService] File upload failed:" << error;

        QVariantMap failedMessage;
        failedMessage["messageId"] = localMessageId;
        failedMessage["type"] = 2;
        failedMessage["conversationId"] = conversationId;
        failedMessage["content"] = "[文件发送失败]";
        failedMessage["senderId"] = m_currentUserId;
        failedMessage["senderName"] = m_currentUserName;
        failedMessage["timestamp"] = currentTimestamp;
        failedMessage["status"] = 3;  // 失败
        failedMessage["fileName"] = QFileInfo(fp).fileName();
        failedMessage["isOffline"] = true;
        failedMessage["errorText"] = error.isEmpty() ? QStringLiteral("文件上传失败，点击重试") : error;
        failedMessage["serverMessageId"] = QString();

        getMessageModel(conversationId)->upsertMessage(failedMessage);
        MessageCache::instance()->cacheMessage(conversationId, failedMessage);

        emit messageReceived(conversationId, failedMessage);
    };

    NetworkClient::instance()->uploadFile(
        "/upload/file",
        filePath,
        "file",
        QJsonObject(),
        progressHandler,
        successHandler,
        errorHandler
    );
}
