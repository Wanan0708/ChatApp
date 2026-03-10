#ifndef MESSAGEPREVIEW_H
#define MESSAGEPREVIEW_H

#include <QString>

namespace MessagePreview {

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