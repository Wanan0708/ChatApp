# PostgreSQL 数据库说明

当前项目以后端服务为数据库入口，客户端不再直接初始化或操作 PostgreSQL。数据库相关文档以 server/init_db.sql 和 server/migrations 为准。

## 当前分工

- database/init.sql: 可选的数据库和用户初始化示例
- server/init_db.sql: 当前项目的基础建表脚本
- server/migrations: 增量迁移脚本
- server/.env: 后端连接数据库的运行配置

## 推荐初始化流程

### 方式一：直接使用本地脚本

Windows 推荐运行：

```powershell
./scripts/init-local-db.ps1
```

### 方式二：手动执行

```bash
createdb -U postgres chatapp
psql -U postgres -d chatapp -f server/init_db.sql
```

如果还需要创建专用数据库用户，可参考 database/init.sql 的写法手动执行。

## 当前主要表

基础表由 server/init_db.sql 创建，当前主要包括：

- users
- conversations
- conversation_members
- messages
- friend_requests
- friends

## 后端配置

server/.env 最少应包含：

```env
PORT=8080
DB_HOST=localhost
DB_PORT=5432
DB_NAME=chatapp
DB_USER=postgres
DB_PASSWORD=123456
JWT_SECRET=replace_me
```

## 常用命令

```bash
# 查看表
psql -U postgres -d chatapp -c "\dt"

# 查看 users 表结构
psql -U postgres -d chatapp -c "\d users"

# 备份数据库
pg_dump -U postgres -d chatapp > backup.sql

# 恢复数据库
psql -U postgres -d chatapp < backup.sql
```

## 与旧文档的区别

- 不再推荐从客户端侧做数据库自动初始化
- 不再把 Qt PostgreSQL 驱动配置当作主流程文档内容
- 不再以 database/init.sql 作为主建表入口

## 常见问题

### 数据库连接失败

优先检查：

1. PostgreSQL 服务是否启动。
2. chatapp 数据库是否已创建。
3. server/.env 中的 DB_HOST、DB_PORT、DB_NAME、DB_USER、DB_PASSWORD 是否正确。

### 表缺失

先执行：

```bash
psql -U postgres -d chatapp -f server/init_db.sql
```

然后再执行迁移脚本。

## 相关文档

- README.md
- docs/运行部署指南.md
- server/migrations/README_MIGRATIONS.md
