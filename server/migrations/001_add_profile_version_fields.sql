-- =====================================================
-- 迁移脚本：001_add_profile_version_fields.sql
-- 版本：1.0
-- 日期：2026-03-07
-- 描述：添加个人资料版本管理字段
-- =====================================================

BEGIN;

-- 添加 profile_version 字段（版本号，每次更新个人资料时 +1）
ALTER TABLE users
ADD COLUMN IF NOT EXISTS profile_version INTEGER DEFAULT 1;

-- 添加 profile_updated_at 字段（个人资料最后更新时间）
ALTER TABLE users
ADD COLUMN IF NOT EXISTS profile_updated_at TIMESTAMP WITH TIME ZONE DEFAULT NOW();

-- 为现有数据设置默认值
UPDATE users 
SET profile_version = 1 
WHERE profile_version IS NULL;

UPDATE users 
SET profile_updated_at = COALESCE(updated_at, created_at, NOW()) 
WHERE profile_updated_at IS NULL;

-- 创建索引以提高查询性能
-- 用于版本号查询优化
CREATE INDEX IF NOT EXISTS idx_users_profile_version 
ON users(profile_version);

-- 用于时间范围查询优化
CREATE INDEX IF NOT EXISTS idx_users_profile_updated_at 
ON users(profile_updated_at);

-- 复合索引：用于批量查询好友更新
CREATE INDEX IF NOT EXISTS idx_users_profile_lookup 
ON users(user_id, profile_version, profile_updated_at);

-- 记录迁移日志
INSERT INTO schema_migrations (version, applied_at) 
VALUES ('001', NOW())
ON CONFLICT (version) DO NOTHING;

COMMIT;

-- 验证迁移结果
SELECT 
    'Migration 001 applied successfully' as status,
    COUNT(*) as users_count,
    COUNT(CASE WHEN profile_version IS NOT NULL THEN 1 END) as users_with_version,
    COUNT(CASE WHEN profile_updated_at IS NOT NULL THEN 1 END) as users_with_updated_at
FROM users;
