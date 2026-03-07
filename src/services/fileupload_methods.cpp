// Sources/services/fileupload_methods.cpp
// 文件上传方法实现

#include "chatservice.h"
#include "../models/messagemodel.h"
#include "../network/networkclient.h"
#include "../network/websocketclient.h"
#include <QDateTime>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonObject>

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
    if (filePath.isEmpty()) return;

    qDebug() << "[ChatService] Sending image message:" << filePath;

    // 创建临时消息（发送中状态）
    QVariantMap tempMessage;
    tempMessage["type"] = 1;  // 图片类型
    tempMessage["conversationId"] = conversationId;
    tempMessage["content"] = "[图片]";
    tempMessage["senderId"] = m_currentUserId;
    tempMessage["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    tempMessage["status"] = 0;  // 发送中
    tempMessage["fileName"] = QFileInfo(filePath).fileName();

    // 立即显示在界面上
    getMessageModel(conversationId)->addMessage(tempMessage);

    // 调用文件上传
    NetworkClient::UploadProgressHandler progressHandler = [](const QJsonObject &, qint64, qint64) {
        // TODO: 更新上传进度
    };

    QString fp = filePath;  // 复制 filePath 到局部变量
    NetworkClient::SuccessHandler successHandler = [this, conversationId, fp](const QJsonObject &result) {
        QString fileId = result.value("fileId").toString();
        QString fileUrl = result.value("fileUrl").toString();
        QString thumbnailUrl = result.value("thumbnailUrl").toString(fileUrl);

        // 创建正式消息
        QVariantMap message;
        message["type"] = 1;  // 图片类型
        message["conversationId"] = conversationId;
        message["content"] = "[图片]";
        message["senderId"] = m_currentUserId;
        message["timestamp"] = QDateTime::currentMSecsSinceEpoch();
        message["status"] = 1;  // 已发送
        message["fileId"] = fileId;
        message["fileName"] = QFileInfo(fp).fileName();
        message["fileUrl"] = fileUrl;
        message["thumbnailUrl"] = thumbnailUrl;

        // 通过 WebSocket 发送
        if (m_webSocketClient->isConnected()) {
            m_webSocketClient->sendMessage(message);
        }

        emit messageReceived(conversationId, message);
    };

    // 修复：在 errorHandler 中也捕获 fp
    NetworkClient::ErrorHandler errorHandler = [this, conversationId, fp](const QString &error) {
        qWarning() << "[ChatService] Image upload failed:" << error;

        // 创建失败消息
        QVariantMap failedMessage;
        failedMessage["type"] = 1;
        failedMessage["conversationId"] = conversationId;
        failedMessage["content"] = "[图片发送失败]";
        failedMessage["senderId"] = m_currentUserId;
        failedMessage["timestamp"] = QDateTime::currentMSecsSinceEpoch();
        failedMessage["status"] = 3;  // 失败
        failedMessage["fileName"] = QFileInfo(fp).fileName();

        emit messageReceived(conversationId, failedMessage);
    };

    NetworkClient::instance()->uploadFile(
        "/api/upload/image",
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
    if (filePath.isEmpty()) return;

    qDebug() << "[ChatService] Sending file message:" << filePath;

    // 创建临时消息（发送中状态）
    QVariantMap tempMessage;
    tempMessage["type"] = 2;  // 文件类型
    tempMessage["conversationId"] = conversationId;
    tempMessage["content"] = "[文件]";
    tempMessage["senderId"] = m_currentUserId;
    tempMessage["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    tempMessage["status"] = 0;  // 发送中
    tempMessage["fileName"] = QFileInfo(filePath).fileName();

    // 立即显示在界面上
    getMessageModel(conversationId)->addMessage(tempMessage);

    // 调用文件上传
    NetworkClient::UploadProgressHandler progressHandler = [](const QJsonObject &, qint64, qint64) {
        // TODO: 更新上传进度
    };

    QString fp = filePath;  // 复制 filePath 到局部变量
    NetworkClient::SuccessHandler successHandler = [this, conversationId, fp](const QJsonObject &result) {
        QString fileId = result.value("fileId").toString();
        QString fileUrl = result.value("fileUrl").toString();
        qint64 fileSize = result.value("fileSize").toVariant().toLongLong();

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

        // 创建正式消息
        QVariantMap message;
        message["type"] = 2;  // 文件类型
        message["conversationId"] = conversationId;
        message["content"] = "[文件]";
        message["senderId"] = m_currentUserId;
        message["timestamp"] = QDateTime::currentMSecsSinceEpoch();
        message["status"] = 1;  // 已发送
        message["fileId"] = fileId;
        message["fileName"] = QFileInfo(fp).fileName();
        message["fileSize"] = sizeStr;
        message["fileUrl"] = fileUrl;

        // 通过 WebSocket 发送
        if (m_webSocketClient->isConnected()) {
            m_webSocketClient->sendMessage(message);
        }

        emit messageReceived(conversationId, message);
    };

    // 修复：在 errorHandler 中也捕获 fp
    NetworkClient::ErrorHandler errorHandler = [this, conversationId, fp](const QString &error) {
        qWarning() << "[ChatService] File upload failed:" << error;

        // 创建失败消息
        QVariantMap failedMessage;
        failedMessage["type"] = 2;
        failedMessage["conversationId"] = conversationId;
        failedMessage["content"] = "[文件发送失败]";
        failedMessage["senderId"] = m_currentUserId;
        failedMessage["timestamp"] = QDateTime::currentMSecsSinceEpoch();
        failedMessage["status"] = 3;  // 失败
        failedMessage["fileName"] = QFileInfo(fp).fileName();

        emit messageReceived(conversationId, failedMessage);
    };

    NetworkClient::instance()->uploadFile(
        "/api/upload/file",
        filePath,
        "file",
        QJsonObject(),
        progressHandler,
        successHandler,
        errorHandler
    );
}
