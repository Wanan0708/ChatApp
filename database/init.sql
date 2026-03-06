-- PostgreSQL 初始化脚本
-- 执行此脚本以创建聊天应用数据库和用户

-- 1. 创建数据库
CREATE DATABASE chatapp
    ENCODING 'UTF8'
    LOCALE 'C'
    TEMPLATE template0;

-- 2. 创建应用用户（建议：不使用 postgres 超级用户）
CREATE USER chatapp_user WITH PASSWORD 'chatapp_password';

-- 3. 授予权限
\c chatapp

GRANT ALL PRIVILEGES ON DATABASE chatapp TO chatapp_user;
GRANT USAGE ON SCHEMA public TO chatapp_user;
GRANT CREATE ON SCHEMA public TO chatapp_user;

ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO chatapp_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON SEQUENCES TO chatapp_user;

-- 提示：修改密码并在应用和环境变量中配置
-- 环境变量示例（保存在 .env 或系统环境中）：
-- CHATAPP_DB_HOST=localhost
-- CHATAPP_DB_NAME=chatapp
-- CHATAPP_DB_USER=chatapp_user
-- CHATAPP_DB_PASSWORD=chatapp_password
-- CHATAPP_DB_PORT=5432
