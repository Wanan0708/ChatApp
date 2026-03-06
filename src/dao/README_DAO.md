# DAO 集成指南

本文档说明如何使用 DAO（数据访问对象）层将聊天应用与 PostgreSQL 数据库集成。

## 架构概览

```
┌─ QML UI ─────────┐
│                  │
└──── ChatService ─── (Singleton)
         │
         ├─ UserDAO
         ├─ ConversationDAO
         └─ MessageDAO
         │
         └─ DatabaseManager ─── PostgreSQL
```

## 核心组件

### 1. DatabaseManager (单例)
- **位置:** `src/database/databasemanager.h/cpp`
- **职责:** PostgreSQL 连接、表初始化、查询执行
- **API:**
  - `connectToDatabase()` - 建立连接，自动创建表
  - `getQuery()` - 获取用于执行参数化查询的 QSqlQuery
  - `beginTransaction()/commitTransaction()` - 事务管理

### 2. UserDAO
- **位置:** `src/dao/userdao.h/cpp`
- **职责:** 用户相关数据库操作
- **API:**
  - `createOrGetUser(userId, username)` - 创建或获取用户
  - `getUserInfo(userId, &username)` - 查询用户信息
  - `getAllUserIds()` - 获取所有用户 ID
  - `generatePasswordResetCode(username)` - 为指定用户名生成临时重置码并存储在 `password_resets` 表中
  - `resetPasswordWithCode(code, newPassword)` - 使用有效的重置码设置新密码，成功后清除记录
  - `updateUserPassword(userId, newPassword)` - 管理员直接将用户密码更新为新值（内部进行哈希）

### 3. ConversationDAO
- **位置:** `src/dao/conversationdao.h/cpp`
- **职责:** 对话相关数据库操作
- **API:**
  - `createConversation(id, name, creatorId)` - 创建对话
  - `getConversation(id, &conv)` - 查询对话
  - `getUserConversations(userId)` - 获取用户的所有对话
  - `addMemberToConversation(convId, userId)` - 添加成员
  - `getConversationMembers(convId)` - 获取对话成员

### 4. MessageDAO
- **位置:** `src/dao/messagedao.h/cpp`
- **职责:** 消息相关数据库操作
- **API:**
  - `saveMessage(message)` - 保存消息到数据库
  - `getConversationMessages(convId, limit)` - 查询对话消息
  - `getMessage(messageId, &msg)` - 查询单个消息
  - `getUnreadMessageCount(convId, userId)` - 获取未读消息数
  - `markMessagesAsRead(convId, userId)` - 标记消息为已读

## ChatService 集成（待实现）

将以下内容集成到 `src/services/chatservice.cpp`:

### 步骤 1: 添加 DAO 成员

```cpp
#include "src/dao/userdao.h"
#include "src/dao/conversationdao.h"
#include "src/dao/messagedao.h"

class ChatService : public QObject {
private:
    UserDAO m_userDAO;
    ConversationDAO m_conversationDAO;
    MessageDAO m_messageDAO;
};
```

### 步骤 2: 替换 initializeSampleData()

**当前（内存）:**
```cpp
void ChatService::initializeSampleData() {
    // 硬编码用户、对话、消息
}
```

**迁移为数据库加载:**
```cpp
void ChatService::initializeSampleData() {
    // 从数据库加载对话
    QVector<Conversation> conversations = m_conversationDAO.getUserConversations("user_1");
    for (const Conversation &conv : conversations) {
        m_conversationModel->addConversation(conv);
    }
}
```

### 步骤 3: 持久化新消息

**修改 sendMessage() 方法:**
```cpp
void ChatService::sendMessage(const QString &content) {
    // 1. 创建消息对象
    Message msg;
    msg.messageId = generateUUID();
    msg.conversationId = m_currentConversationId;
    msg.senderId = m_currentUserId;
    msg.content = content;
    msg.timestamp = QDateTime::currentMSecsSinceEpoch();
    
    // 2. 保存到数据库
    if (!m_messageDAO.saveMessage(msg)) {
        qWarning() << "Failed to save message to database";
        return;
    }
    
    // 3. 添加到本地模型（乐观更新）
    m_messageModel->addMessage(msg);
    
    // 4. 发送到 WebSocket 服务器
    sendMessageViaWebSocket(msg);
}
```

### 步骤 4: 处理接收的消息

**修改 processIncomingMessage() 方法:**
```cpp
void ChatService::processIncomingMessage(const Message &message) {
    // 1. 保存到数据库
    if (!m_messageDAO.saveMessage(message)) {
        qWarning() << "Failed to save received message to database";
        return;
    }
    
    // 2. 添加到本地模型
    if (message.conversationId == m_currentConversationId) {
        m_messageModel->addMessage(message);
    }
    
    // 3. 更新未读计数
    updateUnreadCount(message.conversationId);
}
```

## 数据库表结构

### users
```sql
user_id VARCHAR(50) PRIMARY KEY
username VARCHAR(255) NOT NULL
avatar TEXT
status VARCHAR(20) DEFAULT 'offline'
created_at TIMESTAMP DEFAULT NOW()
updated_at TIMESTAMP DEFAULT NOW()
```

### conversations
```sql
conversation_id VARCHAR(50) PRIMARY KEY
name VARCHAR(255) NOT NULL
type VARCHAR(20) DEFAULT 'single'
creator_id VARCHAR(50) FOREIGN KEY
last_message TEXT
last_message_time TIMESTAMP
created_at TIMESTAMP DEFAULT NOW()
updated_at TIMESTAMP DEFAULT NOW()
```

### conversation_members
```sql
conversation_id VARCHAR(50) NOT NULL
user_id VARCHAR(50) NOT NULL
joined_at TIMESTAMP DEFAULT NOW()
last_read_at TIMESTAMP
PRIMARY KEY (conversation_id, user_id)
```

### messages
```sql
message_id VARCHAR(50) PRIMARY KEY
conversation_id VARCHAR(50) NOT NULL FOREIGN KEY
sender_id VARCHAR(50) NOT NULL FOREIGN KEY
content TEXT NOT NULL
type VARCHAR(20) DEFAULT 'text'
timestamp BIGINT NOT NULL
created_at TIMESTAMP DEFAULT NOW()
```

## 使用示例

### 创建新对话
```cpp
ConversationDAO dao;
dao.createConversation("conv_1", "My Conversation", "user_1");
dao.addMemberToConversation("conv_1", "user_2");
```

### 保存消息
```cpp
MessageDAO dao;
Message msg;
msg.messageId = "msg_1";
msg.conversationId = "conv_1";
msg.senderId = "user_1";
msg.content = "Hello!";
msg.timestamp = QDateTime::currentMSecsSinceEpoch();
dao.saveMessage(msg);
```

### 查询消息
```cpp
MessageDAO dao;
QVector<Message> messages = dao.getConversationMessages("conv_1", 50);
```

### 获取未读计数
```cpp
MessageDAO dao;
int unread = dao.getUnreadMessageCount("conv_1", "user_1");
qDebug() << "Unread messages:" << unread;
```

## 环境变量配置

确保以下环境变量已设置：

```bash
CHATAPP_DB_HOST=localhost
CHATAPP_DB_NAME=chatapp
CHATAPP_DB_USER=chatapp_user
CHATAPP_DB_PASSWORD=your_password
CHATAPP_DB_PORT=5432
```

## 编译和运行

1. **确保 Qt SQL 模块可用:**
   ```
   QT += sql
   ```
   （已在 ChatApp.pro 中配置）

2. **编译项目:**
   ```bash
   qmake ChatApp.pro
   make
   ```

3. **运行应用:**
   ```bash
   ./ChatApp
   ```

## 故障排除

### 连接失败
- 检查 PostgreSQL 服务是否运行
- 验证环境变量是否正确设置
- 查看 application 输出中的错误日志

### 表创建失败
- 确保数据库角色有 CREATE TABLE 权限
- 检查先前的表是否存在（可能需要 DROP）
- 查看 qDebug 输出中的数据库错误

### 参数绑定错误
- 确保在调用 `prepare()` 后才调用 `addBindValue()`
- 验证绑定值的顺序与 SQL 中的 `?` 顺序一致
- 使用 `query.lastError().text()` 查看具体错误

## 后续优化建议

1. **连接池:** 实现连接池以支持并发查询
2. **缓存:** 添加本地缓存以减少数据库查询
3. **批量操作:** 支持批量插入消息（事务）
4. **异步 DAO:** 在后台线程执行数据库操作
5. **ORM 框架:** 考虑集成 Qt SQL 或其他 ORM 库

## 参考资源

- [Qt SQL 文档](https://doc.qt.io/qt-5/qtsql-index.html)
- [PostgreSQL 文档](https://www.postgresql.org/docs/)
- [本项目数据库设置指南](./README_DATABASE.md)
