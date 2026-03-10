#pragma once

#include <QString>

/**
 * @brief 数据库连接配置
 */
struct DatabaseConfig
{
    QString host = defaultHost();
    QString database = "chatapp";
    QString username = "postgres";
    QString password = "123456"; // 生产环境强烈建议通过环境变量修改此密码
    int port = 5432;

    // 从环境变量加载配置
    static DatabaseConfig loadFromEnvironment();

    // 验证配置
    bool isValid() const;

    // 静态工具方法，用于获取后端地址
    static QString defaultHost();
    static QString backendHost();
    static int backendPort();
    static QString backendBaseUrl();
    static QString apiUrl();
    static QString wsUrl();
    static QString resolveServerUrl(const QString &pathOrUrl);
    static QString toServerRelativePath(const QString &pathOrUrl);
};
