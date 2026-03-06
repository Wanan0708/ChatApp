#include "databasemanager.h"
#include "databaseconfig.h"
#include <QSqlDriver>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QCryptographicHash>

DatabaseManager* DatabaseManager::instance()
{
    static DatabaseManager* s_instance = nullptr;
    if (!s_instance) {
        s_instance = new DatabaseManager();
    }
    return s_instance;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    qDebug() << "[DatabaseManager] Instance created";
}

DatabaseManager::~DatabaseManager()
{
    if (m_isConnected) {
        disconnectFromDatabase();
    }
}

bool DatabaseManager::connectToDatabase(const QString &host, const QString &database,
                                        const QString &username, const QString &password,
                                        int port)
{
    // 1. 首先检查并创建数据库（如果不存在）
    if (!ensureDatabaseExists(host, database, username, password, port)) {
        return false;
    }

    // 2. 添加或获取 PostgreSQL 数据库连接
    if (QSqlDatabase::contains("ChatApp")) {
        m_database = QSqlDatabase::database("ChatApp");
    } else {
        m_database = QSqlDatabase::addDatabase("QPSQL", "ChatApp");
    }

    m_database.setHostName(host);
    m_database.setDatabaseName(database);
    m_database.setUserName(username);
    m_database.setPassword(password);
    m_database.setPort(port);

    if (!m_database.open()) {
        m_lastError = QString("[DatabaseManager] Failed to connect: %1").arg(m_database.lastError().text());
        qCritical() << m_lastError;
        m_isConnected = false;
        emit errorOccurred(m_lastError);
        return false;
    }

    m_isConnected = true;
    qDebug() << "[DatabaseManager] Connected to PostgreSQL successfully";
    qDebug() << "  Host:" << host << "Port:" << port << "Database:" << database;
    emit connectionStateChanged(true);

    // 初始化表结构
    if (!initializeTables()) {
        m_lastError = "Failed to initialize database tables";
        qCritical() << m_lastError;
        m_isConnected = false;
        disconnectFromDatabase();
        return false;
    }

    emit databaseInitialized();
    return true;
}

bool DatabaseManager::connectToDatabase()
{
    DatabaseConfig cfg = DatabaseConfig::loadFromEnvironment();
    if (!cfg.isValid()) {
        m_lastError = "Invalid database configuration from environment";
        qWarning() << "[DatabaseManager]" << m_lastError;
        emit errorOccurred(m_lastError);
        return false;
    }

    return connectToDatabase(cfg.host, cfg.database, cfg.username, cfg.password, cfg.port);
}

void DatabaseManager::disconnectFromDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
        m_isConnected = false;
        qDebug() << "[DatabaseManager] Disconnected from PostgreSQL";
        emit connectionStateChanged(false);
    }
}

bool DatabaseManager::isConnected() const
{
    return m_isConnected && m_database.isOpen();
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::initializeTables()
{
    qDebug() << "[DatabaseManager] Initializing database tables...";

    if (!createUserTable()) return false;
    if (!createConversationTable()) return false;
    if (!createConversationMembersTable()) return false;
    if (!createMessageTable()) return false;
    if (!createPasswordResetTable()) return false;
    if (!createFriendsTable()) return false;
    if (!createFriendRequestsTable()) return false;
    if (!createIndexes()) return false;

    qDebug() << "[DatabaseManager] Database tables initialized successfully";
    
    // 插入样本数据
    insertSampleData();
    
    return true;
}

bool DatabaseManager::createUserTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            user_id VARCHAR(50) PRIMARY KEY,
            username VARCHAR(255) NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            avatar TEXT,
            status VARCHAR(20) DEFAULT 'offline',
            created_at TIMESTAMP DEFAULT NOW(),
            updated_at TIMESTAMP DEFAULT NOW()
        )
    )";

    if (!query.exec(sql)) {
        m_lastError = QString("Failed to create users table: %1").arg(query.lastError().text());
        qCritical() << m_lastError;
        return false;
    }

    // 检查并添加缺失的字段（兼容旧表结构）
    QSqlQuery checkQuery(m_database);
    if (checkQuery.exec("SELECT column_name FROM information_schema.columns WHERE table_name='users' AND column_name='password_hash'")) {
        if (!checkQuery.next()) {
            // password_hash 字段不存在，需要添加
            QSqlQuery alterQuery(m_database);
            if (!alterQuery.exec("ALTER TABLE users ADD COLUMN password_hash TEXT NOT NULL DEFAULT ''")) {
                m_lastError = QString("Failed to add password_hash column: %1").arg(alterQuery.lastError().text());
                qCritical() << m_lastError;
                return false;
            }
            qDebug() << "[DatabaseManager] Added password_hash column to users table";
        }
    }

    // 检查username是否为UNIQUE（兼容旧表结构）
    QSqlQuery uniqueCheck(m_database);
    if (uniqueCheck.exec("SELECT COUNT(*) FROM pg_indexes WHERE tablename='users' AND indexdef LIKE '%username%'")) {
        if (uniqueCheck.next() && uniqueCheck.value(0).toInt() == 0) {
            // 添加username唯一约束
            QSqlQuery uniqueQuery(m_database);
            if (!uniqueQuery.exec("ALTER TABLE users ADD CONSTRAINT users_username_unique UNIQUE (username)")) {
                m_lastError = QString("Failed to add username unique constraint: %1").arg(uniqueQuery.lastError().text());
                qCritical() << m_lastError;
                // 继续执行，不强制要求唯一约束
            } else {
                qDebug() << "[DatabaseManager] Added username unique constraint to users table";
            }
        }
    }

    qDebug() << "[DatabaseManager] users table ready";
    return true;
}

bool DatabaseManager::createConversationTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS conversations (
            conversation_id VARCHAR(50) PRIMARY KEY,
            name VARCHAR(255) NOT NULL,
            type VARCHAR(20) DEFAULT 'single',
            creator_id VARCHAR(50),
            last_message TEXT,
            last_message_time TIMESTAMP,
            created_at TIMESTAMP DEFAULT NOW(),
            updated_at TIMESTAMP DEFAULT NOW(),
            FOREIGN KEY (creator_id) REFERENCES users(user_id) ON DELETE SET NULL
        )
    )";

    if (!query.exec(sql)) {
        m_lastError = QString("Failed to create conversations table: %1").arg(query.lastError().text());
        qCritical() << m_lastError;
        return false;
    }

    qDebug() << "[DatabaseManager] conversations table ready";
    return true;
}

bool DatabaseManager::createConversationMembersTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS conversation_members (
            conversation_id VARCHAR(50) NOT NULL,
            user_id VARCHAR(50) NOT NULL,
            joined_at TIMESTAMP DEFAULT NOW(),
            last_read_at TIMESTAMP,
            PRIMARY KEY (conversation_id, user_id),
            FOREIGN KEY (conversation_id) REFERENCES conversations(conversation_id) ON DELETE CASCADE,
            FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
        )
    )";

    if (!query.exec(sql)) {
        m_lastError = QString("Failed to create conversation_members table: %1").arg(query.lastError().text());
        qCritical() << m_lastError;
        return false;
    }

    qDebug() << "[DatabaseManager] conversation_members table ready";
    return true;
}

bool DatabaseManager::createMessageTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS messages (
            message_id VARCHAR(50) PRIMARY KEY,
            conversation_id VARCHAR(50) NOT NULL,
            sender_id VARCHAR(50) NOT NULL,
            content TEXT NOT NULL,
            type VARCHAR(20) DEFAULT 'text',
            timestamp BIGINT NOT NULL,
            created_at TIMESTAMP DEFAULT NOW(),
            FOREIGN KEY (conversation_id) REFERENCES conversations(conversation_id) ON DELETE CASCADE,
            FOREIGN KEY (sender_id) REFERENCES users(user_id) ON DELETE SET NULL
        )
    )";

    if (!query.exec(sql)) {
        m_lastError = QString("Failed to create messages table: %1").arg(query.lastError().text());
        qCritical() << m_lastError;
        return false;
    }

    qDebug() << "[DatabaseManager] messages table ready";
    return true;
}

bool DatabaseManager::createPasswordResetTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS password_resets (
            reset_code VARCHAR(100) PRIMARY KEY,
            user_id VARCHAR(50) NOT NULL,
            expires_at TIMESTAMPTZ NOT NULL,
            created_at TIMESTAMPTZ DEFAULT NOW(),
            FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
        )
    )";

    // 如果旧表存在并且 expires_at 类型不是 TIMESTAMPTZ，则尝试转换
    QSqlQuery typeCheck(m_database);
    if (typeCheck.exec("SELECT data_type FROM information_schema.columns WHERE table_name='password_resets' AND column_name='expires_at'")) {
        if (typeCheck.next()) {
            QString dtype = typeCheck.value(0).toString();
            if (dtype.compare("timestamp without time zone", Qt::CaseInsensitive) == 0) {
                QSqlQuery alter(m_database);
                alter.exec("ALTER TABLE password_resets ALTER COLUMN expires_at TYPE TIMESTAMPTZ USING expires_at AT TIME ZONE 'UTC'");
                qDebug() << "[DatabaseManager] Converted expires_at to TIMESTAMPTZ";
            }
        }
    }

    if (!query.exec(sql)) {
        m_lastError = QString("Failed to create password_resets table: %1").arg(query.lastError().text());
        qCritical() << m_lastError;
        return false;
    }

    qDebug() << "[DatabaseManager] password_resets table ready";
    return true;
}

bool DatabaseManager::createFriendsTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS friends (
            user_id VARCHAR(50) NOT NULL,
            friend_id VARCHAR(50) NOT NULL,
            created_at TIMESTAMP DEFAULT NOW(),
            PRIMARY KEY (user_id, friend_id),
            FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
            FOREIGN KEY (friend_id) REFERENCES users(user_id) ON DELETE CASCADE
        )
    )";

    if (!query.exec(sql)) {
        m_lastError = QString("Failed to create friends table: %1").arg(query.lastError().text());
        qCritical() << m_lastError;
        return false;
    }

    qDebug() << "[DatabaseManager] friends table ready";
    return true;
}

bool DatabaseManager::createFriendRequestsTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS friend_requests (
            request_id SERIAL PRIMARY KEY,
            from_id VARCHAR(50) NOT NULL,
            to_id VARCHAR(50) NOT NULL,
            status VARCHAR(20) DEFAULT 'pending', -- pending, accepted, rejected
            created_at TIMESTAMP DEFAULT NOW(),
            updated_at TIMESTAMP DEFAULT NOW(),
            UNIQUE (from_id, to_id),
            FOREIGN KEY (from_id) REFERENCES users(user_id) ON DELETE CASCADE,
            FOREIGN KEY (to_id) REFERENCES users(user_id) ON DELETE CASCADE
        )
    )";

    if (!query.exec(sql)) {
        m_lastError = QString("Failed to create friend_requests table: %1").arg(query.lastError().text());
        qCritical() << m_lastError;
        return false;
    }

    qDebug() << "[DatabaseManager] friend_requests table ready";
    return true;
}

bool DatabaseManager::createIndexes()
{
    QSqlQuery query(m_database);

    // 密码重置表索引
    QString indexes[] = {
        "CREATE INDEX IF NOT EXISTS idx_messages_conversation ON messages(conversation_id)",
        "CREATE INDEX IF NOT EXISTS idx_messages_sender ON messages(sender_id)",
        "CREATE INDEX IF NOT EXISTS idx_messages_created ON messages(created_at DESC)",
        "CREATE INDEX IF NOT EXISTS idx_members_conversation ON conversation_members(conversation_id)",
        "CREATE INDEX IF NOT EXISTS idx_members_user ON conversation_members(user_id)",
        "CREATE INDEX IF NOT EXISTS idx_password_resets_user ON password_resets(user_id)"
    };

    for (const QString &indexSql : indexes) {
        if (!query.exec(indexSql)) {
            qWarning() << "[DatabaseManager] Warning creating index:" << query.lastError().text();
            // 继续执行，不中断
        }
    }

    qDebug() << "[DatabaseManager] Database indexes created";
    return true;
}

bool DatabaseManager::executeQuery(const QString &queryStr)
{
    if (!isConnected()) {
        m_lastError = "Database is not connected";
        emit errorOccurred(m_lastError);
        return false;
    }

    QSqlQuery query(m_database);
    if (!query.exec(queryStr)) {
        m_lastError = query.lastError().text();
        qWarning() << "[DatabaseManager] Query failed:" << m_lastError;
        emit errorOccurred(m_lastError);
        return false;
    }

    return true;
}

QSqlQuery DatabaseManager::getQuery()
{
    return QSqlQuery(m_database);
}

QSqlQuery DatabaseManager::executeSelectQuery(const QString &queryStr)
{
    QSqlQuery query(m_database);
    if (!query.exec(queryStr)) {
        m_lastError = query.lastError().text();
        qWarning() << "[DatabaseManager] Select query failed:" << m_lastError;
        emit errorOccurred(m_lastError);
    }
    return query;
}

bool DatabaseManager::beginTransaction()
{
    if (!m_database.transaction()) {
        m_lastError = "Failed to begin transaction";
        emit errorOccurred(m_lastError);
        return false;
    }
    return true;
}

bool DatabaseManager::commitTransaction()
{
    if (!m_database.commit()) {
        m_lastError = "Failed to commit transaction";
        emit errorOccurred(m_lastError);
        return false;
    }
    return true;
}

bool DatabaseManager::rollbackTransaction()
{
    if (!m_database.rollback()) {
        m_lastError = "Failed to rollback transaction";
        emit errorOccurred(m_lastError);
        return false;
    }
    return true;
}
bool DatabaseManager::ensureDatabaseExists(const QString &host, const QString &database,
                                          const QString &username, const QString &password,
                                          int port)
{
    // 使用临时连接到 postgres（admin 数据库）来检查和创建目标数据库
    QSqlDatabase tempDb;
    if (QSqlDatabase::contains("PostgreSQLAdmin")) {
        tempDb = QSqlDatabase::database("PostgreSQLAdmin");
    } else {
        tempDb = QSqlDatabase::addDatabase("QPSQL", "PostgreSQLAdmin");
    }

    tempDb.setHostName(host);
    tempDb.setDatabaseName("postgres");  // 连接到 PostgreSQL 的默认 admin 数据库
    tempDb.setUserName(username);
    tempDb.setPassword(password);
    tempDb.setPort(port);

    if (!tempDb.open()) {
        m_lastError = QString("[DatabaseManager] Failed to connect to PostgreSQL admin database: %1")
                      .arg(tempDb.lastError().text());
        qCritical() << m_lastError;
        emit errorOccurred(m_lastError);
        return false;
    }

    qDebug() << "[DatabaseManager] Connected to PostgreSQL admin database";

    // 检查目标数据库是否存在
    QSqlQuery query(tempDb);
    query.prepare("SELECT 1 FROM pg_database WHERE datname = ?");
    query.addBindValue(database);

    if (!query.exec()) {
        m_lastError = QString("[DatabaseManager] Failed to query database list: %1")
                      .arg(query.lastError().text());
        qCritical() << m_lastError;
        tempDb.close();
        return false;
    }

    if (query.next()) {
        // 数据库已存在
        qDebug() << "[DatabaseManager] Database" << database << "already exists";
        tempDb.close();
        return true;
    }

    // 数据库不存在，需要创建
    qDebug() << "[DatabaseManager] Database" << database << "does not exist, creating...";

    // 禁用自动提交以创建数据库（某些 PostgreSQL 驱动可能需要）
    query.exec("CREATE DATABASE " + database + ";");

    if (query.lastError().isValid() && !query.lastError().text().contains("already exists", Qt::CaseInsensitive)) {
        m_lastError = QString("[DatabaseManager] Failed to create database: %1")
                      .arg(query.lastError().text());
        qCritical() << m_lastError;
        tempDb.close();
        emit errorOccurred(m_lastError);
        return false;
    }

    qDebug() << "[DatabaseManager] Database" << database << "created successfully";
    tempDb.close();
    return true;
}

bool DatabaseManager::insertSampleData()
{
    qDebug() << "[DatabaseManager] Inserting sample data...";

    // 检查是否已有数据，避免重复插入
    QSqlQuery checkQuery(m_database);
    checkQuery.prepare("SELECT COUNT(*) FROM users;");
    if (!checkQuery.exec()) {
        qWarning() << "[DatabaseManager] Failed to check existing data";
        return false;
    }

    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        qDebug() << "[DatabaseManager] Sample data already exists, skipping insertion";
        return true;
    }

    // 开启事务
    if (!beginTransaction()) {
        qWarning() << "[DatabaseManager] Failed to begin transaction for sample data";
        return false;
    }

    // 1. 插入用户 - 使用程序化哈希计算
    auto hashPassword = [](const QString &password) -> QString {
        QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
        return hash.toHex();
    };

    struct UserData { QString id, name, password; };
    QVector<UserData> users = {
        {"user_001", "Alice", "alice123"},
        {"user_002", "Bob", "bob123"},
        {"user_003", "Charlie", "charlie123"}
    };

    for (const auto &user : users) {
        QString passwordHash = hashPassword(user.password);
        qDebug() << "[DatabaseManager] User:" << user.name << "Password:" << user.password << "Hash:" << passwordHash;
        QString insertSql = QString("INSERT INTO users (user_id, username, password_hash, status, created_at, updated_at) "
                                   "VALUES ('%1', '%2', '%3', 'online', NOW(), NOW());")
                                   .arg(user.id)
                                   .arg(user.name)
                                   .arg(passwordHash);
        
        if (!executeQuery(insertSql)) {
            qWarning() << "[DatabaseManager] Failed to insert user:" << lastError();
            rollbackTransaction();
            return false;
        }
    }
    qDebug() << "[DatabaseManager] Inserted" << users.size() << "users";

    // 定义当前时间戳
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    // 2. 插入对话
    struct ConvData { QString id, name, type, creator, lastMessage; qint64 lastMessageTime; };
    QVector<ConvData> convs = {
        {"conv_001", "Alice & Bob", "single", "user_001", "还不错，你呢？", now - 40000},
        {"conv_002", "Bob & Charlie", "single", "user_002", "在的，Bob。有什么事吗？", now - 110000},
        {"conv_003", "Group Chat", "group", "user_001", "好的，我们讨论今天的任务", now - 150000}
    };

    for (const auto &conv : convs) {
        QString insertSql = QString("INSERT INTO conversations (conversation_id, name, type, creator_id, last_message, last_message_time, created_at, updated_at) "
                                   "VALUES ('%1', '%2', '%3', '%4', '%5', TO_TIMESTAMP(%6/1000.0), NOW(), NOW());")
                                   .arg(conv.id)
                                   .arg(conv.name)
                                   .arg(conv.type)
                                   .arg(conv.creator)
                                   .arg(conv.lastMessage)
                                   .arg(QString::number(conv.lastMessageTime));
        
        if (!executeQuery(insertSql)) {
            qWarning() << "[DatabaseManager] Failed to insert conversation:" << lastError();
            rollbackTransaction();
            return false;
        }
    }
    qDebug() << "[DatabaseManager] Inserted" << convs.size() << "conversations";

    // 3. 插入对话成员
    struct MemberData { QString convId, userId; };
    QVector<MemberData> members = {
        {"conv_001", "user_001"}, {"conv_001", "user_002"},
        {"conv_002", "user_002"}, {"conv_002", "user_003"},
        {"conv_003", "user_001"}, {"conv_003", "user_002"}, {"conv_003", "user_003"}
    };

    for (const auto &member : members) {
        QString insertSql = QString("INSERT INTO conversation_members (conversation_id, user_id, joined_at) "
                                   "VALUES ('%1', '%2', NOW());")
                                   .arg(member.convId)
                                   .arg(member.userId);
        
        if (!executeQuery(insertSql)) {
            qWarning() << "[DatabaseManager] Failed to insert member:" << lastError();
            rollbackTransaction();
            return false;
        }
    }
    qDebug() << "[DatabaseManager] Inserted" << members.size() << "conversation members";

    // 4. 插入消息
    struct MessageData { QString id, convId, senderId, content; qint64 ts; };
    QVector<MessageData> messages = {
        {"msg_001", "conv_001", "user_001", "你好，Bob！", now - 60000},
        {"msg_002", "conv_001", "user_002", "你好，Alice！最近怎么样？", now - 50000},
        {"msg_003", "conv_001", "user_001", "还不错，你呢？", now - 40000},
        
        {"msg_004", "conv_002", "user_002", "Charlie，在吗？", now - 120000},
        {"msg_005", "conv_002", "user_003", "在的，Bob。有什么事吗？", now - 110000},
        
        {"msg_006", "conv_003", "user_001", "大家好，召集在线人员开会", now - 180000},
        {"msg_007", "conv_003", "user_002", "收到，现在开始吗？", now - 170000},
        {"msg_008", "conv_003", "user_003", "可以，我也在线", now - 160000},
        {"msg_009", "conv_003", "user_001", "好的，我们讨论今天的任务", now - 150000}
    };

    for (const auto &msg : messages) {
        QString insertSql = QString("INSERT INTO messages (message_id, conversation_id, sender_id, content, type, timestamp, created_at) "
                                   "VALUES ('%1', '%2', '%3', '%4', 'text', %5, NOW());")
                                   .arg(msg.id)
                                   .arg(msg.convId)
                                   .arg(msg.senderId)
                                   .arg(msg.content)
                                   .arg(QString::number(msg.ts));

        if (!executeQuery(insertSql)) {
            qWarning() << "[DatabaseManager] Failed to insert message:" << lastError();
            rollbackTransaction();
            return false;
        }
    }
    qDebug() << "[DatabaseManager] Inserted" << messages.size() << "messages";

    // 提交事务
    if (!commitTransaction()) {
        qWarning() << "[DatabaseManager] Failed to commit transaction for sample data";
        return false;
    }

    qDebug() << "[DatabaseManager] Sample data inserted successfully";
    return true;
}
