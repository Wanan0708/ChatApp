#include "logger.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

Logger* Logger::instance()
{
    static Logger instance;
    return &instance;
}

Logger::Logger(QObject *parent)
    : QObject(parent)
    , m_logFile(nullptr)
    , m_logStream(nullptr)
    , m_minLogLevel(LogLevel::Debug)
    , m_logToFile(true)
    , m_logToConsole(true)
{
    // 创建日志目录
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    QDir().mkpath(logDir);
    
    QString logPath = logDir + "/eschat.log";
    
    // 检查是否需要轮转
    QFileInfo fileInfo(logPath);
    if (fileInfo.exists() && fileInfo.size() > MAX_LOG_SIZE) {
        rotateLogFile();
    }
    
    // 打开日志文件
    m_logFile = new QFile(logPath, this);
    if (m_logFile->open(QIODevice::Append | QIODevice::Text)) {
        m_logStream = new QTextStream(m_logFile);
    }
}

Logger::~Logger()
{
    if (m_logStream) {
        m_logStream->flush();
    }
    if (m_logFile && m_logFile->isOpen()) {
        m_logFile->close();
    }
}

void Logger::debug(const QString &message, const QString &category)
{
    writeLog(LogLevel::Debug, message, category);
}

void Logger::info(const QString &message, const QString &category)
{
    writeLog(LogLevel::Info, message, category);
}

void Logger::warning(const QString &message, const QString &category)
{
    writeLog(LogLevel::Warning, message, category);
}

void Logger::error(const QString &message, const QString &category)
{
    writeLog(LogLevel::Error, message, category);
}

void Logger::critical(const QString &message, const QString &category)
{
    writeLog(LogLevel::Critical, message, category);
}

void Logger::setLogLevel(LogLevel level)
{
    m_minLogLevel = level;
}

void Logger::setLogToFile(bool enabled)
{
    m_logToFile = enabled;
}

void Logger::setLogToConsole(bool enabled)
{
    m_logToConsole = enabled;
}

QString Logger::logFilePath() const
{
    if (m_logFile) {
        return m_logFile->fileName();
    }
    return "";
}

void Logger::clearLog()
{
    QMutexLocker locker(&m_mutex);
    if (m_logFile && m_logFile->isOpen()) {
        m_logFile->resize(0);
    }
}

void Logger::writeLog(LogLevel level, const QString &message, const QString &category)
{
    if (level < m_minLogLevel) {
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    
    QString logEntry = formatLogEntry(level, message, category);
    
    // 输出到控制台
    if (m_logToConsole) {
#ifdef QT_DEBUG
        qDebug() << logEntry;
#else
        if (level >= LogLevel::Warning) {
            qDebug() << logEntry;
        }
#endif
    }
    
    // 输出到文件
    if (m_logToFile && m_logStream && m_logFile->isOpen()) {
        *m_logStream << logEntry << Qt::endl;
        m_logStream->flush();
        
        // 检查是否需要轮转
        if (m_logFile->size() > MAX_LOG_SIZE) {
            rotateLogFile();
        }
    }
    
    // 发出信号
    emit logEntryAdded(logLevelToString(level), message, category, QDateTime::currentMSecsSinceEpoch());
}

QString Logger::formatLogEntry(LogLevel level, const QString &message, const QString &category) const
{
    QJsonObject logObj;
    logObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    logObj["level"] = logLevelToString(level);
    logObj["message"] = message;
    if (!category.isEmpty()) {
        logObj["category"] = category;
    }
    
    return QJsonDocument(logObj).toJson(QJsonDocument::Compact);
}

QString Logger::logLevelToString(LogLevel level) const
{
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

void Logger::rotateLogFile()
{
    if (!m_logFile) return;
    
    if (m_logStream) {
        m_logStream->flush();
    }
    if (m_logFile->isOpen()) {
        m_logFile->close();
    }
    
    QString logPath = m_logFile->fileName();
    QString logDir = QFileInfo(logPath).absolutePath();
    
    // 删除最旧的日志文件
    QString oldestLog = logDir + "/eschat.log." + QString::number(MAX_LOG_FILES);
    if (QFile::exists(oldestLog)) {
        QFile::remove(oldestLog);
    }
    
    // 轮转现有日志文件
    for (int i = MAX_LOG_FILES - 1; i >= 1; --i) {
        QString oldPath = logDir + "/eschat.log." + QString::number(i);
        QString newPath = logDir + "/eschat.log." + QString::number(i + 1);
        if (QFile::exists(oldPath)) {
            QFile::rename(oldPath, newPath);
        }
    }
    
    // 重命名当前日志文件
    QString newPath = logPath + ".1";
    if (QFile::exists(logPath)) {
        QFile::rename(logPath, newPath);
    }
    
    // 重新打开日志文件
    m_logFile->setFileName(logPath);
    if (m_logFile->open(QIODevice::Append | QIODevice::Text)) {
        if (!m_logStream) {
            m_logStream = new QTextStream(m_logFile);
        } else {
            m_logStream->setDevice(m_logFile);
        }
    }
}
