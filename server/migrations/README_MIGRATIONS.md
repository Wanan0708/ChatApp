# 数据库迁移指南

当前迁移目录只负责 server/init_db.sql 之后的增量变更，不替代基础建表脚本。

## 当前目录

```text
server/migrations/
├── 000_create_schema_migrations_table.sql
├── 001_add_profile_version_fields.sql
├── 002_add_extended_user_profile_fields.sql
├── rollback_001_add_profile_version_fields.sql
├── rollback_002_add_extended_user_profile_fields.sql
├── run_migrations.bat
└── run_migrations.sh
```

## 使用顺序

### 新数据库

先执行基础建表：

```bash
psql -U postgres -d chatapp -f server/init_db.sql
```

再执行迁移：

```bat
server\migrations\run_migrations.bat
```

### 已有数据库升级

先备份，再执行迁移脚本：

```bash
pg_dump -U postgres -d chatapp > backup.sql
```

```bat
server\migrations\run_migrations.bat
```

## 推荐方式

### Windows

```bat
server\migrations\run_migrations.bat
```

### Linux/macOS

```bash
cd server
./migrations/run_migrations.sh
```

脚本会：

- 检查 psql 是否存在
- 检查数据库连接
- 自动创建 schema_migrations 表
- 按序执行未应用的迁移文件

## 当前迁移说明

### 000_create_schema_migrations_table.sql

创建 schema_migrations 表，用于记录迁移版本。

### 001_add_profile_version_fields.sql

为 users 表补充：

- profile_version
- profile_updated_at
- 相关索引

### 002_add_extended_user_profile_fields.sql

为 users 表补充：

- gender
- region
- phone
- email
- contact
- bio
- age
- status
- last_seen
- 在线状态相关索引

### 003_add_message_media_fields.sql

为 messages 表补充：

- status
- file_id
- file_name
- file_size
- file_url
- thumbnail_url

用于支持图片/文件消息上传、转发和历史记录回放。

### 004_add_message_recalled_flag.sql

为 messages 表补充：

- recalled

用于支持消息撤回状态同步、历史记录回放以及客户端本地缓存中的服务端撤回映射。

## 常用检查命令

```bash
psql -U postgres -d chatapp -c "SELECT * FROM schema_migrations ORDER BY applied_at;"
psql -U postgres -d chatapp -c "\d users"
```

## 命名约定

迁移文件使用：

```text
NNN_description.sql
```

回滚文件使用：

```text
rollback_NNN_description.sql
```

## 注意事项

- 迁移不是建库入口，建库仍以 server/init_db.sql 为准
- 新增迁移前先确认基础脚本是否已经覆盖该字段或表
- 生产环境执行迁移前必须先备份

## 故障排除

### 连接失败

检查以下环境变量：

- CHATAPP_DB_HOST
- CHATAPP_DB_PORT
- CHATAPP_DB_NAME
- CHATAPP_DB_USER
- CHATAPP_DB_PASSWORD

### 迁移重复执行

如果版本已记录在 schema_migrations 中，脚本会跳过对应迁移。

## 相关文档

- database/README_DATABASE.md
- docs/运行部署指南.md
- README.md
