#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <QObject>
#include <QString>
#include <QQueue>
#include <QTimer>
#include <QDateTime>

/**
 * @brief 错误类型枚举
 */
enum class ErrorType {
    Network,        // 网络错误
    Auth,           // 认证错误
    Server,         // 服务器错误
    Client,         // 客户端错误
    Validation,     // 验证错误
    Unknown         // 未知错误
};

/**
 * @brief 错误级别枚举
 */
enum class ErrorLevel {
    Info,           // 提示信息
    Warning,        // 警告
    Critical,       // 严重错误
    Fatal           // 致命错误
};

/**
 * @brief 错误结构体
 */
struct ErrorInfo {
    ErrorType type;
    ErrorLevel level;
    QString code;
    QString message;
    QString details;
    qint64 timestamp;
    
    ErrorInfo() : type(ErrorType::Unknown), level(ErrorLevel::Info), timestamp(QDateTime::currentMSecsSinceEpoch()) {}
};

/**
 * @brief 全局错误处理器
 * 
 * 统一管理应用中的所有错误
 * - 错误分类和级别
 * - 错误队列
 * - 错误通知
 * - 错误日志
 */
class ErrorHandler : public QObject
{
    Q_OBJECT
public:
    static ErrorHandler* instance();

    // 报告错误
    Q_INVOKABLE void reportError(const QString &message, ErrorType type = ErrorType::Unknown, 
                                  ErrorLevel level = ErrorLevel::Info, const QString &details = "");
    
    // 便捷方法
    Q_INVOKABLE void reportNetworkError(const QString &message, const QString &details = "");
    Q_INVOKABLE void reportAuthError(const QString &message, const QString &details = "");
    Q_INVOKABLE void reportServerError(const QString &message, const QString &details = "");
    Q_INVOKABLE void reportValidationError(const QString &message, const QString &details = "");
    
    // 清除错误
    Q_INVOKABLE void clearErrors();
    Q_INVOKABLE void dismissCurrentError();
    
    // 获取当前错误
    Q_INVOKABLE ErrorInfo currentError() const;
    Q_INVOKABLE bool hasError() const;

signals:
    void errorOccurred(const ErrorInfo &error);
    void errorDismissed();
    void errorCountChanged(int count);

private slots:
    void processErrorQueue();

private:
    explicit ErrorHandler(QObject *parent = nullptr);
    ~ErrorHandler() override;

    QString errorTypeToString(ErrorType type) const;
    QString errorLevelToString(ErrorLevel level) const;

    QQueue<ErrorInfo> m_errorQueue;
    ErrorInfo m_currentError;
    QTimer *m_queueTimer;
    bool m_processing = false;
};

#endif // ERRORHANDLER_H
