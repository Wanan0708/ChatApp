#include "inputvalidator.h"
#include "../core/logger.h"
#include <QRegularExpression>
#include <QStringList>

InputValidator* InputValidator::instance()
{
    static InputValidator instance;
    return &instance;
}

InputValidator::InputValidator(QObject *parent)
    : QObject(parent)
{
    Logger::instance()->debug("InputValidator initialized", "validator");
}

InputValidator::~InputValidator()
{
}

bool InputValidator::validateUsername(const QString &username) const
{
    if (username.isEmpty()) {
        return false;
    }
    
    // 长度检查
    if (!validateLength(username, MIN_USERNAME_LENGTH, MAX_USERNAME_LENGTH)) {
        return false;
    }
    
    // 只能包含字母、数字、下划线、连字符
    QRegularExpression re("^[a-zA-Z0-9_-]+$");
    if (!re.match(username).hasMatch()) {
        return false;
    }
    
    // 不能以特殊字符开头
    if (username.startsWith('_') || username.startsWith('-')) {
        return false;
    }
    
    // 检查是否包含 XSS 或 SQL 注入
    if (containsXSS(username) || containsSQLInjection(username)) {
        Logger::instance()->warning(QString("Suspicious username detected: %1").arg(username), "security");
        return false;
    }
    
    return true;
}

bool InputValidator::validatePassword(const QString &password) const
{
    if (password.isEmpty()) {
        return false;
    }
    
    // 长度检查
    if (!validateLength(password, MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH)) {
        return false;
    }
    
    return true;
}

bool InputValidator::validateEmail(const QString &email) const
{
    if (email.isEmpty()) {
        return false;
    }
    
    // RFC 5322 简化版邮箱验证
    QRegularExpression re(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return re.match(email).hasMatch();
}

bool InputValidator::validatePhoneNumber(const QString &phone) const
{
    if (phone.isEmpty()) {
        return false;
    }
    
    // 支持多种手机号格式
    QRegularExpression re(R"(^(\+?86)?1[3-9]\d{9}$)");
    return re.match(phone.trimmed()).hasMatch();
}

int InputValidator::checkPasswordStrength(const QString &password) const
{
    int strength = 0;
    
    if (password.length() >= 8) strength++;
    if (password.length() >= 12) strength++;
    
    // 包含小写字母
    if (QRegularExpression("[a-z]").match(password).hasMatch()) strength++;
    
    // 包含大写字母
    if (QRegularExpression("[A-Z]").match(password).hasMatch()) strength++;
    
    // 包含数字
    if (QRegularExpression("[0-9]").match(password).hasMatch()) strength++;
    
    // 包含特殊字符
    if (QRegularExpression("[^a-zA-Z0-9]").match(password).hasMatch()) strength++;
    
    // 归一化到 0-4
    return qMin(4, strength / 2);
}

QString InputValidator::getPasswordStrengthText(int strength) const
{
    switch (strength) {
        case 0: return "非常弱";
        case 1: return "弱";
        case 2: return "中等";
        case 3: return "强";
        case 4: return "非常强";
        default: return "未知";
    }
}

QString InputValidator::sanitizeInput(const QString &input) const
{
    if (input.isEmpty()) {
        return input;
    }
    
    QString sanitized = input;
    
    // 移除控制字符
    sanitized.remove(QRegularExpression("[\\x00-\\x08\\x0B\\x0C\\x0E-\\x1F\\x7F]"));
    
    // 移除 XSS
    sanitized = removeXSS(sanitized);
    
    // SQL 转义
    sanitized = escapeSql(sanitized);
    
    // 去除首尾空白
    sanitized = sanitized.trimmed();
    
    return sanitized;
}

QString InputValidator::removeXSS(const QString &input) const
{
    if (input.isEmpty()) {
        return input;
    }
    
    QString result = input;
    
    // 移除 script 标签
    result.replace(QRegularExpression("<script[^>]*>", QRegularExpression::CaseInsensitiveOption), "");
    result.replace(QRegularExpression("</script>", QRegularExpression::CaseInsensitiveOption), "");
    
    // 移除 javascript: 协议
    result.replace(QRegularExpression("javascript:", QRegularExpression::CaseInsensitiveOption), "");
    
    // 移除 on* 事件处理器
    result.replace(QRegularExpression("\\s*on\\w+\\s*=\\s*['\"][^'\"]*['\"]", QRegularExpression::CaseInsensitiveOption), "");
    
    // 移除 iframe
    result.replace(QRegularExpression("<iframe[^>]*>", QRegularExpression::CaseInsensitiveOption), "");
    result.replace(QRegularExpression("</iframe>", QRegularExpression::CaseInsensitiveOption), "");
    
    // 移除 object/embed
    result.replace(QRegularExpression("<object[^>]*>", QRegularExpression::CaseInsensitiveOption), "");
    result.replace(QRegularExpression("<embed[^>]*>", QRegularExpression::CaseInsensitiveOption), "");
    
    return result;
}

QString InputValidator::escapeSql(const QString &input) const
{
    if (input.isEmpty()) {
        return input;
    }
    
    QString result = input;
    
    // 转义单引号
    result.replace("'", "''");
    
    // 转义反斜杠
    result.replace("\\", "\\\\");
    
    // 转义特殊字符
    result.replace("\n", "\\n");
    result.replace("\r", "\\r");
    result.replace("\t", "\\t");
    
    return result;
}

bool InputValidator::validateLength(const QString &input, int min, int max) const
{
    if (input.isEmpty()) {
        return min == 0;
    }
    
    int len = input.length();
    return len >= min && len <= max;
}

QString InputValidator::getValidationError(const QString &input, const QString &type) const
{
    if (type == "username") {
        if (input.isEmpty()) return "用户名不能为空";
        if (input.length() < MIN_USERNAME_LENGTH) return QString("用户名至少 %1 个字符").arg(MIN_USERNAME_LENGTH);
        if (input.length() > MAX_USERNAME_LENGTH) return QString("用户名最多 %1 个字符").arg(MAX_USERNAME_LENGTH);
        if (!QRegularExpression("^[a-zA-Z0-9_-]+$").match(input).hasMatch()) return "用户名只能包含字母、数字、下划线和连字符";
        return "无效的用户名";
    }
    
    if (type == "password") {
        if (input.isEmpty()) return "密码不能为空";
        if (input.length() < MIN_PASSWORD_LENGTH) return QString("密码至少 %1 个字符").arg(MIN_PASSWORD_LENGTH);
        if (input.length() > MAX_PASSWORD_LENGTH) return QString("密码最多 %1 个字符").arg(MAX_PASSWORD_LENGTH);
        return "无效的密码";
    }
    
    if (type == "email") {
        if (input.isEmpty()) return "邮箱不能为空";
        return "无效的邮箱格式";
    }
    
    if (type == "phone") {
        if (input.isEmpty()) return "手机号不能为空";
        return "无效的手机号格式";
    }
    
    return "输入无效";
}

bool InputValidator::containsXSS(const QString &input) const
{
    // 检查常见的 XSS 模式
    QStringList xssPatterns = {
        "<script", "javascript:", "onerror=", "onload=", "onclick=",
        "onmouseover=", "onfocus=", "onblur=", "<iframe", "<object",
        "<embed", "<svg", "expression(", "vbscript:"
    };
    
    QString lowerInput = input.toLower();
    for (const QString &pattern : xssPatterns) {
        if (lowerInput.contains(pattern)) {
            return true;
        }
    }
    
    return false;
}

bool InputValidator::containsSQLInjection(const QString &input) const
{
    // 检查常见的 SQL 注入模式
    QStringList sqlPatterns = {
        "' OR '1'='1", "\" OR \"1\"=\"1", "'; DROP TABLE", "--",
        "UNION SELECT", "INSERT INTO", "DELETE FROM", "UPDATE.*SET",
        "EXEC(", "EXECUTE(", "xp_", "sp_"
    };
    
    QString upperInput = input.toUpper();
    for (const QString &pattern : sqlPatterns) {
        if (upperInput.contains(pattern.toUpper())) {
            return true;
        }
    }
    
    return false;
}
