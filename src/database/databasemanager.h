#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariantMap>
#include <QList>

/**
 * @brief PostgreSQL 数据库管理器（单例模式）
 * 
 * 功能：
 * - 管理数据库连接
 * - 初始化表结构
 * - 提供数据库操作接口
 * - 错误处理和日志记录
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager* instance();
    
    // 数据库连接管理
    bool connectToDatabase(); // 从环境加载配置并连接
    bool connectToDatabase(const QString &host, const QString &database,
                          const QString &username, const QString &password,
                          int port = 5432);
    void disconnectFromDatabase();
    bool isConnected() const;
    QString lastError() const;

    // 表初始化
    bool initializeTables();
    
    // 样本数据初始化
    bool insertSampleData();

    // 数据库操作接口
    QSqlQuery getQuery();  // 获取一个新的 QSqlQuery 对象用于绑定参数
    bool executeQuery(const QString &queryStr);
    QSqlQuery executeSelectQuery(const QString &queryStr);
    
    // 添加获取数据库连接的方法
    QSqlDatabase database() const { return m_database; }
    
    // 事务管理
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

signals:
    void connectionStateChanged(bool connected);
    void errorOccurred(const QString &error);
    void databaseInitialized();

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager() override;
    
    // 禁止复制
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager& operator=(const DatabaseManager &) = delete;

    // 创建表的私有方法
    bool createUserTable();
    bool createConversationTable();
    bool createConversationMembersTable();
    bool createMessageTable();
    bool createPasswordResetTable();
    bool createFriendsTable();
    bool createFriendRequestsTable();
    bool createIndexes();
    
    // 数据库初始化辅助方法
    bool ensureDatabaseExists(const QString &host, const QString &database,
                             const QString &username, const QString &password,
                             int port);

    // 成员变量
    QSqlDatabase m_database;
    QString m_lastError;
    bool m_isConnected = false;
};
