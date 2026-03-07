#ifndef SECURESTORAGE_H
#define SECURESTORAGE_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

/**
 * @brief 安全存储工具类
 * 
 * 提供跨平台的敏感数据加密存储功能
 * - Windows: 使用 DPAPI 加密
 * - macOS: 使用 Keychain（待实现）
 * - Linux: 使用 KWallet/GNOME Keyring（待实现）
 * - 通用：使用简单加密（基于机器码）
 */
class SecureStorage : public QObject
{
    Q_OBJECT
public:
    static SecureStorage* instance();

    // 存储敏感数据
    Q_INVOKABLE void setValue(const QString &key, const QString &value);
    Q_INVOKABLE QString value(const QString &key, const QString &defaultValue = "") const;
    Q_INVOKABLE void remove(const QString &key);
    Q_INVOKABLE void clear();

    // 专门用于 Token 存储
    Q_INVOKABLE void setAuthToken(const QString &token);
    Q_INVOKABLE QString authToken() const;
    Q_INVOKABLE void clearAuthToken();

    // 专门用于密码存储
    Q_INVOKABLE void setEncryptedPassword(const QString &username, const QString &password);
    Q_INVOKABLE QString encryptedPassword(const QString &username) const;
    Q_INVOKABLE void clearEncryptedPassword(const QString &username);

signals:
    void tokenChanged();
    void passwordChanged();

private:
    explicit SecureStorage(QObject *parent = nullptr);
    ~SecureStorage() override;

    // 加密/解密方法
    QString encrypt(const QString &plainText) const;
    QString decrypt(const QString &encryptedText) const;
    
    // 生成机器码作为密钥
    QString getMachineId() const;

    QSettings *m_settings;
    QString m_cacheKey;
};

#endif // SECURESTORAGE_H
