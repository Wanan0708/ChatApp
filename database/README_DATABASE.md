# PostgreSQL 数据库配置指南

## 🔧 前置要求

1. **安装 PostgreSQL**（版本 10+）
   - Windows: https://www.postgresql.org/download/windows/
   - Linux: `sudo apt-get install postgresql postgresql-contrib`
   - macOS: `brew install postgresql`

2. **Qt PostgreSQL 驱动**
   - Windows: 通常包含在 Qt 安装中
   - Linux: `sudo apt-get install libpq-dev`
   - macOS: `brew install libpq`

## 📋 初始化数据库

### 方法 1：使用 SQL 脚本（推荐）

```bash
# 1. 连接到 PostgreSQL
psql -U postgres

# 2. 执行初始化脚本
\i database/init.sql

# 3. 验证
\c chatapp
\dt  # 列出所有表
```

### 方法 2：手动操作

```bash
# 创建数据库和用户
createdb -U postgres chatapp
psql -U postgres -d chatapp -c "CREATE USER chatapp_user WITH PASSWORD 'chatapp_password';"
psql -U postgres -d chatapp -c "GRANT ALL PRIVILEGES ON DATABASE chatapp TO chatapp_user;"
```

## 🔐 环境变量配置

### Windows（使用系统环境变量）

```powershell
setx CHATAPP_DB_HOST localhost
setx CHATAPP_DB_NAME chatapp
setx CHATAPP_DB_USER chatapp_user
setx CHATAPP_DB_PASSWORD your_password
setx CHATAPP_DB_PORT 5432
```

### Linux/macOS（添加到 ~/.bashrc 或 ~/.zshrc）

```bash
export CHATAPP_DB_HOST=localhost
export CHATAPP_DB_NAME=chatapp
export CHATAPP_DB_USER=chatapp_user
export CHATAPP_DB_PASSWORD=your_password
export CHATAPP_DB_PORT=5432
```

## 🚀 在代码中使用

### 初始化数据库连接

```cpp
#include "src/database/databasemanager.h"
#include "src/database/databaseconfig.h"

// 在 main() 中
DatabaseConfig config = DatabaseConfig::loadFromEnvironment();
DatabaseManager* dbManager = DatabaseManager::instance();

if (!dbManager->connectToDatabase(config.host, config.database, 
                                 config.username, config.password, 
                                 config.port)) {
    qCritical() << "Database connection failed:" << dbManager->lastError();
    return -1;
}

qDebug() << "Database ready!";
```

### 执行查询

```cpp
// 执行 INSERT/UPDATE/DELETE
dbManager->executeQuery("INSERT INTO users (id, name, created_at) VALUES ('user1', 'Alice', 1707213600000)");

// 执行 SELECT
QSqlQuery result = dbManager->executeSelectQuery("SELECT * FROM users WHERE id = 'user1'");
while (result.next()) {
    QString name = result.value("name").toString();
    qDebug() << name;
}
```

## 📊 数据库架构

### 表结构

#### users（用户表）
```
id (PK)          - 用户ID
name             - 用户名
avatar           - 头像URL
status           - 状态 (online/offline/busy)
created_at       - 创建时间（时间戳）
updated_at       - 更新时间（时间戳）
```

#### conversations（会话表）
```
id (PK)          - 会话ID
title            - 会话标题
type             - 类型 (user/group)
owner_id (FK)    - 创建者ID
last_message     - 最后一条消息
last_message_time - 最后消息时间
created_at       - 创建时间
updated_at       - 更新时间
```

#### conversation_members（会话成员表）
```
conversation_id (FK, PK) - 会话ID
user_id (FK, PK)         - 用户ID
unread_count             - 未读消息数
joined_at                - 加入时间
```

#### messages（消息表）
```
id (PK)              - 消息ID
conversation_id (FK) - 会话ID
sender_id (FK)       - 发送者ID
content              - 消息内容
type                 - 类型 (text/image/file)
is_offline           - 是否离线消息
created_at           - 创建时间
```

## 🔍 常用命令

```bash
# 连接到数据库
psql -U chatapp_user -d chatapp -h localhost

# 查看表结构
\d users
\d conversations

# 查看所有表
\dt

# 导出数据
pg_dump -U chatapp_user -d chatapp > backup.sql

# 导入数据
psql -U chatapp_user -d chatapp < backup.sql

# 重置数据库
dropdb -U postgres chatapp
createdb -U postgres chatapp
```

## ⚠️ 常见问题

### 连接失败：连接被拒绝
- 检查 PostgreSQL 服务是否运行
- 验证主机名、用户名、密码
- 检查防火墙设置

### 找不到 QPSQL 驱动
- 确保已安装 Qt PostgreSQL 插件
- 检查 Qt 的 plugins/sqldrivers 目录

### 权限错误
- 确保用户已授予相应权限（见 init.sql）
- 尝试使用 postgres 超级用户执行初始化

## 📝 下一步

- 集成 DAOs（Data Access Objects）处理数据库操作
- 修改 ChatService 使用数据库而非硬编码数据
- 添加数据迁移和备份机制
