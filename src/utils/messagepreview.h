#ifndef MESSAGEPREVIEW_H
#define MESSAGEPREVIEW_H

#include <QString>

namespace MessagePreview {

inline QString recalledPreview(bool isSelf)
{
    return isSelf
        ? QStringLiteral("你撤回了一条消息")
        : QStringLiteral("对方撤回了一条消息");
}

inline QString recalledPreviewForConversation(bool isSelf, const QString &conversationType, const QString &senderName)
{
    if (isSelf) {
        return QStringLiteral("你撤回了一条消息");
    }

    if (conversationType == QStringLiteral("single") || conversationType == QStringLiteral("user") || conversationType.isEmpty()) {
        return QStringLiteral("对方撤回了一条消息");
    }

    const QString normalizedSenderName = senderName.trimmed();
    if (!normalizedSenderName.isEmpty()) {
        return QStringLiteral("%1撤回了一条消息").arg(normalizedSenderName);
    }

    return QStringLiteral("有成员撤回了一条消息");
}

inline QString normalizeConversationPreview(const QString &content, int messageType = -1)
{
    QString preview = content.trimmed();

    if (messageType == 1) {
        preview = QStringLiteral("图片");
    } else if (messageType == 2) {
        preview = QStringLiteral("文件");
    } else if (preview == QStringLiteral("[图片]")) {
        preview = QStringLiteral("图片");
    } else if (preview == QStringLiteral("[文件]")) {
        preview = QStringLiteral("文件");
    } else if (preview == QStringLiteral("[图片发送失败]")) {
        preview = QStringLiteral("图片发送失败");
    } else if (preview == QStringLiteral("[文件发送失败]")) {
        preview = QStringLiteral("文件发送失败");
    }

    if (preview.length() > 30) {
        preview = preview.left(27) + QStringLiteral("...");
    }

    return preview;
}

}

#endif // MESSAGEPREVIEW_H