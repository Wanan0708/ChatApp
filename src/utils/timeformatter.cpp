#include "timeformatter.h"
#include <QDateTime>

QString TimeFormatter::formatChatTime(qint64 timestamp)
{
    return formatChatTime(QDateTime::fromMSecsSinceEpoch(timestamp));
}

QString TimeFormatter::formatChatTime(const QDateTime &dateTime)
{
    QDateTime now = QDateTime::currentDateTime();
    QDate messageDate = dateTime.date();
    QDate today = now.date();
    
    // 计算消息日期与今天的天数差
    qint64 daysDiff = messageDate.daysTo(today);
    
    if (daysDiff == 0) {
        // 今天的消息：显示时分
        return dateTime.toString("HH:mm");
    } else if (daysDiff == 1) {
        // 昨天的消息
        return "昨天";
    } else if (daysDiff > 1 && daysDiff <= 6) {
        // 本周内的消息（2-6天前）：显示周几
        // 0=周一, 1=周二, ..., 6=周日
        static const QString weekDays[] = {"周一", "周二", "周三", "周四", "周五", "周六", "周日"};
        int dayOfWeek = messageDate.dayOfWeek() - 1;  // Qt的dayOfWeek() 返回1-7
        if (dayOfWeek < 0) dayOfWeek = 6;  // 周日处理
        return weekDays[dayOfWeek];
    } else {
        // 更早的消息
        if (messageDate.year() == today.year()) {
            // 同年：显示月日
            return messageDate.toString("M月d日");
        } else {
            // 不同年份：显示完整日期
            return messageDate.toString("yyyy-MM-dd");
        }
    }
}
