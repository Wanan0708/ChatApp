#!/bin/bash
# =====================================================
# 数据库迁移执行脚本
# 用法：./run_migrations.sh [database_name]
# =====================================================

set -e

# 配置
DB_HOST="${CHATAPP_DB_HOST:-localhost}"
DB_PORT="${CHATAPP_DB_PORT:-5432}"
DB_NAME="${1:-${CHATAPP_DB_NAME:-chatapp}}"
DB_USER="${CHATAPP_DB_USER:-postgres}"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}   ChatApp 数据库迁移工具${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# 检查 psql 是否可用
if ! command -v psql &> /dev/null; then
    echo -e "${RED}错误：未找到 psql 命令${NC}"
    echo "请确保 PostgreSQL 已安装并添加到 PATH"
    exit 1
fi

# 检查数据库连接
echo -e "${YELLOW}检查数据库连接...${NC}"
if ! psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" -c "SELECT 1" &> /dev/null; then
    echo -e "${RED}错误：无法连接到数据库${NC}"
    echo "主机：$DB_HOST:$DB_PORT"
    echo "数据库：$DB_NAME"
    echo "用户：$DB_USER"
    echo ""
    echo "请检查："
    echo "  1. PostgreSQL 服务是否运行"
    echo "  2. 数据库是否存在"
    echo "  3. 用户名密码是否正确"
    exit 1
fi
echo -e "${GREEN}✓ 数据库连接成功${NC}"
echo ""

# 获取脚本所在目录
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# 获取已应用的迁移版本
echo -e "${YELLOW}检查迁移历史...${NC}"

# 创建迁移历史表（如果不存在）
psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" \
    -f "$SCRIPT_DIR/000_create_schema_migrations_table.sql" &> /dev/null || true

# 获取已应用的版本
APPLIED_VERSIONS=$(psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" -t -c \
    "SELECT version FROM schema_migrations ORDER BY version;" 2>/dev/null | tr -d ' ')

echo -e "${GREEN}✓ 已应用的迁移：${NC}${APPLIED_VERSIONS:-无}"
echo ""

# 获取待应用的迁移文件
MIGRATION_FILES=$(ls "$SCRIPT_DIR"/[0-9][0-9][0-9]_*.sql 2>/dev/null | sort)

if [ -z "$MIGRATION_FILES" ]; then
    echo -e "${YELLOW}未找到迁移文件${NC}"
    exit 0
fi

# 应用每个迁移
echo -e "${YELLOW}开始应用迁移...${NC}"
echo ""

for migration_file in $MIGRATION_FILES; do
    filename=$(basename "$migration_file")
    version=$(echo "$filename" | grep -oE '^[0-9]+' | sed 's/^0*//')
    
    # 检查是否已应用
    if echo "$APPLIED_VERSIONS" | grep -q "^${version}$"; then
        echo -e "${YELLOW}⊘ 跳过：$filename (已应用)${NC}"
        continue
    fi
    
    echo -e "${GREEN}→ 应用：$filename${NC}"
    
    # 执行迁移
    if psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" -f "$migration_file"; then
        echo -e "${GREEN}✓ 成功：$filename${NC}"
    else
        echo -e "${RED}✗ 失败：$filename${NC}"
        echo -e "${RED}迁移中止！请检查错误信息。${NC}"
        exit 1
    fi
    echo ""
done

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}   迁移完成！${NC}"
echo -e "${GREEN}========================================${NC}"

# 显示最终状态
echo ""
echo -e "${YELLOW}当前迁移状态：${NC}"
psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" -c \
    "SELECT version, applied_at FROM schema_migrations ORDER BY version;"
