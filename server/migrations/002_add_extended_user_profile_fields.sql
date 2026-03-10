-- =====================================================
-- 迁移脚本：002_add_extended_user_profile_fields.sql
-- 版本：1.0
-- 日期：2026-03-09
-- 描述：补充 users 表扩展资料字段与在线状态字段
-- =====================================================

BEGIN;

-- 补充用户资料字段，兼容旧库升级
ALTER TABLE users
ADD COLUMN IF NOT EXISTS gender VARCHAR(20),
ADD COLUMN IF NOT EXISTS region VARCHAR(100),
ADD COLUMN IF NOT EXISTS phone VARCHAR(50),
ADD COLUMN IF NOT EXISTS email VARCHAR(255),
ADD COLUMN IF NOT EXISTS contact VARCHAR(255),
ADD COLUMN IF NOT EXISTS bio TEXT,
ADD COLUMN IF NOT EXISTS age INTEGER DEFAULT 25,
ADD COLUMN IF NOT EXISTS status VARCHAR(20) DEFAULT 'offline',
ADD COLUMN IF NOT EXISTS last_seen TIMESTAMP WITH TIME ZONE DEFAULT NOW();

-- 为旧数据补齐默认值，避免接口读取空值
UPDATE users
SET age = 25
WHERE age IS NULL;

UPDATE users
SET status = 'offline'
WHERE status IS NULL OR BTRIM(status) = '';

UPDATE users
SET last_seen = COALESCE(updated_at, created_at, NOW())
WHERE last_seen IS NULL;

-- 在线状态查询会频繁按状态和最后活跃时间读取
CREATE INDEX IF NOT EXISTS idx_users_status ON users(status);
CREATE INDEX IF NOT EXISTS idx_users_last_seen ON users(last_seen DESC);

-- 记录迁移日志
INSERT INTO schema_migrations (version, applied_at)
VALUES ('002', NOW())
ON CONFLICT (version) DO NOTHING;

COMMIT;

-- 验证迁移结果
SELECT
    'Migration 002 applied successfully' AS status,
    COUNT(*) AS users_count,
    COUNT(CASE WHEN age IS NOT NULL THEN 1 END) AS users_with_age,
    COUNT(CASE WHEN status IS NOT NULL THEN 1 END) AS users_with_status,
    COUNT(CASE WHEN last_seen IS NOT NULL THEN 1 END) AS users_with_last_seen
FROM users;