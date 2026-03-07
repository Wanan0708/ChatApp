#include "errorhandler.h"
#include <QDebug>
#include <QDateTime>

ErrorHandler* ErrorHandler::instance()
{
    static ErrorHandler instance;
    return &instance;
}

ErrorHandler::ErrorHandler(QObject *parent)
    : QObject(parent)
    , m_queueTimer(new QTimer(this))
{
    connect(m_queueTimer, &QTimer::timeout, this, &ErrorHandler::processErrorQueue);
    m_queueTimer->setSingleShot(true);
    m_queueTimer->setInterval(100);  // 100ms 延迟处理队列
}

ErrorHandler::~ErrorHandler()
{
}

void ErrorHandler::reportError(const QString &message, ErrorType type, ErrorLevel level, const QString &details)
{
    ErrorInfo error;
    error.type = type;
    error.level = level;
    error.message = message;
    error.details = details;
    error.code = errorTypeToString(type) + "_" + QString::number(static_cast<int>(level));
    error.timestamp = QDateTime::currentMSecsSinceEpoch();
    
    // 添加到错误队列
    m_errorQueue.enqueue(error);
    
    // 启动队列处理定时器
    if (!m_processing) {
        m_queueTimer->start();
    }
    
    emit errorCountChanged(m_errorQueue.size());
}

void ErrorHandler::reportNetworkError(const QString &message, const QString &details)
{
    reportError(message, ErrorType::Network, ErrorLevel::Warning, details);
}

void ErrorHandler::reportAuthError(const QString &message, const QString &details)
{
    reportError(message, ErrorType::Auth, ErrorLevel::Critical, details);
}

void ErrorHandler::reportServerError(const QString &message, const QString &details)
{
    reportError(message, ErrorType::Server, ErrorLevel::Critical, details);
}

void ErrorHandler::reportValidationError(const QString &message, const QString &details)
{
    reportError(message, ErrorType::Validation, ErrorLevel::Info, details);
}

void ErrorHandler::clearErrors()
{
    m_errorQueue.clear();
    m_currentError = ErrorInfo();
    emit errorDismissed();
    emit errorCountChanged(0);
}

void ErrorHandler::dismissCurrentError()
{
    m_currentError = ErrorInfo();
    emit errorDismissed();
    
    // 处理下一个错误
    if (!m_errorQueue.isEmpty()) {
        m_queueTimer->start();
    }
}

ErrorInfo ErrorHandler::currentError() const
{
    return m_currentError;
}

bool ErrorHandler::hasError() const
{
    return m_currentError.type != ErrorType::Unknown || !m_errorQueue.isEmpty();
}

void ErrorHandler::processErrorQueue()
{
    if (m_errorQueue.isEmpty()) {
        m_processing = false;
        return;
    }
    
    m_processing = true;
    
    // 如果有当前错误正在显示，不处理新错误
    if (m_currentError.type != ErrorType::Unknown) {
        m_queueTimer->start(3000);  // 3 秒后重试
        return;
    }
    
    // 取出队列中的第一个错误
    m_currentError = m_errorQueue.dequeue();
    emit errorCountChanged(m_errorQueue.size());
    
    // 输出到调试日志
    qDebug() << "[ErrorHandler]" 
             << errorLevelToString(m_currentError.level)
             << "[" << errorTypeToString(m_currentError.type) << "]"
             << m_currentError.message;
    
    if (!m_currentError.details.isEmpty()) {
        qDebug() << "[ErrorHandler] Details:" << m_currentError.details;
    }
    
    // 通知 QML 显示错误
    emit errorOccurred(m_currentError);
    
    // 根据错误级别设置自动消失时间
    int autoDismissDelay = 0;
    switch (m_currentError.level) {
        case ErrorLevel::Info:
            autoDismissDelay = 3000;
            break;
        case ErrorLevel::Warning:
            autoDismissDelay = 5000;
            break;
        case ErrorLevel::Critical:
            autoDismissDelay = 0;  // 需要用户手动关闭
            break;
        case ErrorLevel::Fatal:
            autoDismissDelay = 0;  // 需要用户手动关闭
            break;
    }
    
    if (autoDismissDelay > 0) {
        QTimer::singleShot(autoDismissDelay, this, [this]() {
            dismissCurrentError();
        });
    }
}

QString ErrorHandler::errorTypeToString(ErrorType type) const
{
    switch (type) {
        case ErrorType::Network: return "NETWORK";
        case ErrorType::Auth: return "AUTH";
        case ErrorType::Server: return "SERVER";
        case ErrorType::Client: return "CLIENT";
        case ErrorType::Validation: return "VALIDATION";
        default: return "UNKNOWN";
    }
}

QString ErrorHandler::errorLevelToString(ErrorLevel level) const
{
    switch (level) {
        case ErrorLevel::Info: return "INFO";
        case ErrorLevel::Warning: return "WARNING";
        case ErrorLevel::Critical: return "CRITICAL";
        case ErrorLevel::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}
