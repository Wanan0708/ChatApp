#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <QObject>
#include <QRegularExpression>
#include <QString>

/**
 * @brief 输入验证器
 * 
 * 提供用户输入验证和 XSS/SQL 注入防护
 * 
 * 特性：
 * - 用户名验证
 * - 密码强度检查
 * - 邮箱验证
 * - 手机号验证
 * - XSS 防护
 * - SQL 注入防护
 */
class InputValidator : public QObject
{
    Q_OBJECT
public:
    static InputValidator* instance();

    // 基础验证
    Q_INVOKABLE bool validateUsername(const QString &username) const;
    Q_INVOKABLE bool validatePassword(const QString &password) const;
    Q_INVOKABLE bool validateEmail(const QString &email) const;
    Q_INVOKABLE bool validatePhoneNumber(const QString &phone) const;
    
    // 密码强度
    Q_INVOKABLE int checkPasswordStrength(const QString &password) const;  // 0-4
    Q_INVOKABLE QString getPasswordStrengthText(int strength) const;
    
    // 安全过滤
    Q_INVOKABLE QString sanitizeInput(const QString &input) const;
    Q_INVOKABLE QString removeXSS(const QString &input) const;
    Q_INVOKABLE QString escapeSql(const QString &input) const;
    
    // 长度验证
    Q_INVOKABLE bool validateLength(const QString &input, int min, int max) const;
    
    // 错误信息
    Q_INVOKABLE QString getValidationError(const QString &input, const QString &type) const;

private:
    explicit InputValidator(QObject *parent = nullptr);
    ~InputValidator() override;

    bool containsXSS(const QString &input) const;
    bool containsSQLInjection(const QString &input) const;

    static const int MIN_USERNAME_LENGTH = 3;
    static const int MAX_USERNAME_LENGTH = 32;
    static const int MIN_PASSWORD_LENGTH = 6;
    static const int MAX_PASSWORD_LENGTH = 64;
};

#endif // INPUTVALIDATOR_H
