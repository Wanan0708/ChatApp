# 数据库迁移指南

## 📁 目录结构

```
server/migrations/
├── 000_create_schema_migrations_table.sql   # 创建迁移历史表（首次执行）
├── 001_add_profile_version_fields.sql       # 添加个人资料版本字段
└── rollback_001_add_profile_version_fields.sql  # 回滚脚本
```

## 🚀 快速开始

### 1. 首次设置（新数据库）

```bash
# 进入服务器目录
cd server

# 执行迁移历史表创建
psql -U postgres -h localhost -d chatapp -f migrations/000_create_schema_migrations_table.sql

# 执行所有迁移（按顺序）
psql -U postgres -h localhost -d chatapp -f migrations/001_add_profile_version_fields.sql
```

### 2. 已有数据库升级

```bash
# 1. 备份数据库（重要！）
pg_dump -U postgres -h localhost chatapp > backup_$(date +%Y%m%d_%H%M%S).sql

# 2. 执行迁移
psql -U postgres -h localhost -d chatapp -f migrations/001_add_profile_version_fields.sql

# 3. 验证
psql -U postgres -h localhost -d chatapp -c "\d users"
```

### 3. Windows PowerShell 脚本

```powershell
# 设置变量
$env:PGUSER = "postgres"
$env:PGHOST = "localhost"
$env:PGDATABASE = "chatapp"

# 执行迁移
psql -f migrations/000_create_schema_migrations_table.sql
psql -f migrations/001_add_profile_version_fields.sql

# 验证
psql -c "\d users"
```

## 📋 迁移命令参考

### 查看当前数据库结构

```bash
# 查看 users 表结构
psql -U postgres -h localhost -d chatapp -c "\d users"

# 查看所有字段
psql -U postgres -h localhost -d chatapp -c "
    SELECT column_name, data_type, column_default, is_nullable
    FROM information_schema.columns
    WHERE table_name = 'users'
    ORDER BY ordinal_position;
"

# 查看迁移历史
psql -U postgres -h localhost -d chatapp -c "SELECT * FROM schema_migrations ORDER BY applied_at;"
```

### 应用迁移

```bash
# 应用单个迁移
psql -U postgres -h localhost -d chatapp -f migrations/001_add_profile_version_fields.sql

# 应用所有未执行的迁移（脚本）
./run_migrations.sh  # Linux/macOS
```

### 回滚迁移

```bash
# 回滚单个迁移
psql -U postgres -h localhost -d chatapp -f migrations/rollback_001_add_profile_version_fields.sql
```

## 🔍 验证迁移结果

### 1. 检查字段是否存在

```sql
SELECT column_name, data_type, column_default
FROM information_schema.columns
WHERE table_name = 'users'
  AND column_name IN ('profile_version', 'profile_updated_at');
```

### 2. 检查索引是否创建

```sql
SELECT indexname, indexdef
FROM pg_indexes
WHERE tablename = 'users'
  AND indexname LIKE 'idx_users_profile%';
```

### 3. 检查数据是否正确

```sql
SELECT 
    user_id,
    username,
    profile_version,
    profile_updated_at,
    created_at
FROM users
LIMIT 10;
```

## ⚠️ 注意事项

### 生产环境部署

1. **必须先备份**
   ```bash
   pg_dump -U postgres -h localhost chatapp | gzip > backup_$(date +%Y%m%d).sql.gz
   ```

2. **在测试环境验证**
   - 先在测试环境执行迁移
   - 验证应用功能正常
   - 再部署到生产环境

3. **维护窗口**
   - 选择低峰期执行
   - 大表迁移可能需要较长时间

4. **回滚计划**
   - 准备好回滚脚本
   - 测试回滚流程

### 开发环境

```bash
# 快速重置数据库
dropdb -U postgres chatapp
createdb -U postgres chatapp
psql -U postgres -h localhost -d chatapp -f ../server/init_db.sql
psql -U postgres -h localhost -d chatapp -f migrations/000_create_schema_migrations_table.sql
psql -U postgres -h localhost -d chatapp -f migrations/001_add_profile_version_fields.sql
```

## 📝 迁移文件命名规范

```
NNN_description.sql
```

- `NNN`: 三位数字序号（001, 002, 003...）
- `description`: 小写字母和下划线，描述迁移内容

示例：
- `001_add_profile_version_fields.sql`
- `002_add_user_avatar_index.sql`
- `003_create_groups_table.sql`

## 🔧 故障排除

### 问题：权限错误

```bash
# 确保用户有足够权限
psql -U postgres -d chatapp -c "GRANT ALL ON ALL TABLES IN SCHEMA public TO your_user;"
```

### 问题：列已存在

迁移脚本使用 `ADD COLUMN IF NOT EXISTS`，会安全跳过已存在的列。

### 问题：连接失败

```bash
# 检查 PostgreSQL 服务
# Windows: 服务管理器中查看 PostgreSQL 服务
# Linux: systemctl status postgresql

# 检查连接
psql -U postgres -h localhost -c "SELECT version();"
```

## 📚 相关文档

- [PostgreSQL ALTER TABLE 文档](https://www.postgresql.org/docs/current/sql-altertable.html)
- [PostgreSQL 索引文档](https://www.postgresql.org/docs/current/sql-createindex.html)
- 项目数据库文档：`database/README_DATABASE.md`
