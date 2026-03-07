-- =====================================================
-- 回滚脚本：rollback_001_add_profile_version_fields.sql
-- 版本：1.0
-- 日期：2026-03-07
-- 描述：回滚个人资料版本管理字段迁移
-- =====================================================

BEGIN;

-- 删除索引
DROP INDEX IF EXISTS idx_users_profile_lookup;
DROP INDEX IF EXISTS idx_users_profile_updated_at;
DROP INDEX IF EXISTS idx_users_profile_version;

-- 删除字段
ALTER TABLE users DROP COLUMN IF EXISTS profile_version;
ALTER TABLE users DROP COLUMN IF EXISTS profile_updated_at;

-- 删除迁移记录
DELETE FROM schema_migrations WHERE version = '001';

COMMIT;

-- 验证回滚结果
SELECT 
    'Migration 001 rolled back successfully' as status,
    COUNT(*) as users_count
FROM users;
