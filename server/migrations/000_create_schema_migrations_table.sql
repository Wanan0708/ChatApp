-- =====================================================
-- 迁移历史表：schema_migrations
-- 描述：记录已应用的数据库迁移版本
-- =====================================================

-- 创建迁移历史表
CREATE TABLE IF NOT EXISTS schema_migrations (
    version VARCHAR(50) PRIMARY KEY,
    applied_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
    description TEXT,
    checksum VARCHAR(64)
);

-- 创建索引
CREATE INDEX IF NOT EXISTS idx_schema_migrations_applied_at 
ON schema_migrations(applied_at);

-- 插入初始记录（可选）
COMMENT ON TABLE schema_migrations IS 'Database migration history tracking';
