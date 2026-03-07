#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QStandardPaths>

/**
 * @brief 日志级别枚举
 */
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

/**
 * @brief 结构化日志管理器
 * 
 * 提供统一的日志记录功能
 * - 日志级别过滤
 * - 日志文件轮转
 * - 结构化日志格式
 * - 日志持久化
 */
class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger* instance();

    // 日志方法
    Q_INVOKABLE void debug(const QString &message, const QString &category = "");
    Q_INVOKABLE void info(const QString &message, const QString &category = "");
    Q_INVOKABLE void warning(const QString &message, const QString &category = "");
    Q_INVOKABLE void error(const QString &message, const QString &category = "");
    Q_INVOKABLE void critical(const QString &message, const QString &category = "");

    // 配置
    Q_INVOKABLE void setLogLevel(LogLevel level);
    Q_INVOKABLE void setLogToFile(bool enabled);
    Q_INVOKABLE void setLogToConsole(bool enabled);
    
    // 日志文件管理
    Q_INVOKABLE QString logFilePath() const;
    Q_INVOKABLE void clearLog();

signals:
    void logEntryAdded(const QString &level, const QString &message, const QString &category, qint64 timestamp);

private:
    explicit Logger(QObject *parent = nullptr);
    ~Logger() override;

    void writeLog(LogLevel level, const QString &message, const QString &category);
    QString formatLogEntry(LogLevel level, const QString &message, const QString &category) const;
    QString logLevelToString(LogLevel level) const;
    void rotateLogFile();

    QFile *m_logFile;
    QTextStream *m_logStream;
    QMutex m_mutex;
    
    LogLevel m_minLogLevel;
    bool m_logToFile;
    bool m_logToConsole;
    
    static const int MAX_LOG_SIZE = 10 * 1024 * 1024;  // 10MB
    static const int MAX_LOG_FILES = 5;
};

#endif // LOGGER_H
