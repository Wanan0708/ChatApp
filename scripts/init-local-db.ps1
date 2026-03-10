Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent $scriptDir
$initSql = Join-Path $projectRoot "server\init_db.sql"

if (-not (Test-Path $initSql)) {
    throw "未找到初始化脚本: $initSql"
}

if (-not (Get-Command psql -ErrorAction SilentlyContinue)) {
    throw "未找到 psql，请先安装 PostgreSQL 并将其加入 PATH"
}

$dbName = if ($env:CHATAPP_DB_NAME) { $env:CHATAPP_DB_NAME } else { "chatapp" }
$dbUser = if ($env:CHATAPP_DB_USER) { $env:CHATAPP_DB_USER } else { "postgres" }
$dbHost = if ($env:CHATAPP_DB_HOST) { $env:CHATAPP_DB_HOST } else { "localhost" }
$dbPort = if ($env:CHATAPP_DB_PORT) { $env:CHATAPP_DB_PORT } else { "5432" }

Write-Host "[ESChat] 初始化数据库: $dbName"

$exists = & psql -h $dbHost -p $dbPort -U $dbUser -tAc "SELECT 1 FROM pg_database WHERE datname = '$dbName'" postgres
if ($LASTEXITCODE -ne 0) {
    throw "检查数据库是否存在失败，请确认连接参数和密码正确"
}

if (-not $exists.Trim()) {
    Write-Host "[ESChat] 创建数据库 $dbName"
    & createdb -h $dbHost -p $dbPort -U $dbUser $dbName
    if ($LASTEXITCODE -ne 0) {
        throw "创建数据库失败: $dbName"
    }
}

Write-Host "[ESChat] 执行基础建表脚本"
& psql -h $dbHost -p $dbPort -U $dbUser -d $dbName -f $initSql
if ($LASTEXITCODE -ne 0) {
    throw "执行 server/init_db.sql 失败"
}

Write-Host "[ESChat] 基础数据库初始化完成"