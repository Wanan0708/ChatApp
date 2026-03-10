-- =====================================================
-- 回滚脚本：rollback_002_add_extended_user_profile_fields.sql
-- 版本：1.0
-- 日期：2026-03-09
-- 描述：回滚 users 表扩展资料字段与在线状态字段
-- =====================================================

BEGIN;

DROP INDEX IF EXISTS idx_users_last_seen;
DROP INDEX IF EXISTS idx_users_status;

ALTER TABLE users
DROP COLUMN IF EXISTS last_seen,
DROP COLUMN IF EXISTS status,
DROP COLUMN IF EXISTS age,
DROP COLUMN IF EXISTS bio,
DROP COLUMN IF EXISTS contact,
DROP COLUMN IF EXISTS email,
DROP COLUMN IF EXISTS phone,
DROP COLUMN IF EXISTS region,
DROP COLUMN IF EXISTS gender;

DELETE FROM schema_migrations
WHERE version = '002';

COMMIT;