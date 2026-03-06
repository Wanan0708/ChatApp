#ifndef TIMEFORMATTER_H
#define TIMEFORMATTER_H

#include <QString>
#include <QDateTime>

/**
 * @brief 聊天应用时间格式化工具
 * 
 * 显示规则：
 * - 今天的消息：HH:mm（如 10:30）
 * - 昨天的消息：显示"昨天"
 * - 本周消息：显示周几（如 周一）
 * - 更早的消息：显示月日（如 2月1日）
 * - 更早的消息（不同年）：显示完整日期（如 2025-02-01）
 */
class TimeFormatter
{
public:
    /**
     * @brief 格式化消息时间戳为聊天显示格式
     * @param timestamp 毫秒级时间戳
     * @return 格式化后的时间字符串
     */
    static QString formatChatTime(qint64 timestamp);

    /**
     * @brief 格式化消息时间为聊天显示格式
     * @param dateTime QDateTime 对象
     * @return 格式化后的时间字符串
     */
    static QString formatChatTime(const QDateTime &dateTime);

private:
    TimeFormatter() = default;
};

#endif // TIMEFORMATTER_H
