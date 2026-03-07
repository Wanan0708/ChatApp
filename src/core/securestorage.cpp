#include "securestorage.h"
#include <QCryptographicHash>
#include <QNetworkInterface>
#include <QDebug>

SecureStorage* SecureStorage::instance()
{
    static SecureStorage instance;
    return &instance;
}

SecureStorage::SecureStorage(QObject *parent)
    : QObject(parent)
{
    // 配置文件存储路径
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configPath);
    
    m_settings = new QSettings(configPath + "/secure.ini", QSettings::IniFormat, this);
    m_cacheKey = getMachineId();
    
    qDebug() << "[SecureStorage] Initialized with config path:" << configPath;
}

SecureStorage::~SecureStorage()
{
    m_settings->sync();
}

QString SecureStorage::getMachineId() const
{
    // 使用 MAC 地址生成机器唯一标识
    QString macAddress;
    const auto interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        if (iface.flags() & QNetworkInterface::IsUp &&
            !(iface.flags() & QNetworkInterface::IsLoopBack)) {
            macAddress = iface.hardwareAddress();
            break;
        }
    }
    
    if (macAddress.isEmpty()) {
        macAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }
    
    // 生成 SHA256 哈希作为密钥
    QByteArray hash = QCryptographicHash::hash(macAddress.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(hash.toHex());
}

QString SecureStorage::encrypt(const QString &plainText) const
{
    if (plainText.isEmpty()) return "";
    
    // 简单 XOR 加密（实际生产环境应使用更安全的加密方式）
    QByteArray data = plainText.toUtf8();
    QByteArray key = m_cacheKey.left(32).toUtf8();  // 使用机器码前 32 位作为密钥
    
    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ key[i % key.size()];
    }
    
    // 转换为 Base64 便于存储
    return QString::fromLatin1(data.toBase64());
}

QString SecureStorage::decrypt(const QString &encryptedText) const
{
    if (encryptedText.isEmpty()) return "";
    
    // Base64 解码
    QByteArray data = QByteArray::fromBase64(encryptedText.toLatin1());
    QByteArray key = m_cacheKey.left(32).toUtf8();
    
    // XOR 解密（与加密相同）
    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ key[i % key.size()];
    }
    
    return QString::fromUtf8(data);
}

void SecureStorage::setValue(const QString &key, const QString &value)
{
    m_settings->setValue(key, value);
    m_settings->sync();
}

QString SecureStorage::value(const QString &key, const QString &defaultValue) const
{
    return m_settings->value(key, defaultValue).toString();
}

void SecureStorage::remove(const QString &key)
{
    m_settings->remove(key);
    m_settings->sync();
}

void SecureStorage::clear()
{
    m_settings->clear();
    m_settings->sync();
}

void SecureStorage::setAuthToken(const QString &token)
{
    // 加密存储 Token
    QString encryptedToken = encrypt(token);
    m_settings->setValue("auth/token", encryptedToken);
    m_settings->sync();
    emit tokenChanged();
    qDebug() << "[SecureStorage] Auth token stored securely";
}

QString SecureStorage::authToken() const
{
    QString encryptedToken = m_settings->value("auth/token", "").toString();
    if (encryptedToken.isEmpty()) return "";
    
    QString token = decrypt(encryptedToken);
    return token;
}

void SecureStorage::clearAuthToken()
{
    m_settings->remove("auth/token");
    m_settings->sync();
    emit tokenChanged();
}

void SecureStorage::setEncryptedPassword(const QString &username, const QString &password)
{
    // 加密存储密码
    QString encryptedPassword = encrypt(password);
    m_settings->setValue("credentials/" + username, encryptedPassword);
    m_settings->sync();
    emit passwordChanged();
    qDebug() << "[SecureStorage] Password stored securely for user:" << username;
}

QString SecureStorage::encryptedPassword(const QString &username) const
{
    QString encryptedPassword = m_settings->value("credentials/" + username, "").toString();
    if (encryptedPassword.isEmpty()) return "";
    
    return decrypt(encryptedPassword);
}

void SecureStorage::clearEncryptedPassword(const QString &username)
{
    m_settings->remove("credentials/" + username);
    m_settings->sync();
}
