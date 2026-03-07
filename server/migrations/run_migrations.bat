@echo off
REM =====================================================
REM 数据库迁移执行脚本 (Windows)
REM 用法：run_migrations.bat [database_name]
REM =====================================================

SETLOCAL EnableDelayedExpansion

REM 配置
SET DB_HOST=%CHATAPP_DB_HOST%
IF "%DB_HOST%"=="" SET DB_HOST=localhost

SET DB_PORT=%CHATAPP_DB_PORT%
IF "%DB_PORT%"=="" SET DB_PORT=5432

SET DB_NAME=%1
IF "%DB_NAME%"=="" SET DB_NAME=%CHATAPP_DB_NAME%
IF "%DB_NAME%"=="" SET DB_NAME=chatapp

SET DB_USER=%CHATAPP_DB_USER%
IF "%DB_USER%"=="" SET DB_USER=postgres

SET PGUSER=%DB_USER%
SET PGPASSWORD=%CHATAPP_DB_PASSWORD%

echo ========================================
echo    ChatApp 数据库迁移工具 (Windows)
echo ========================================
echo.

REM 检查 psql 是否可用
where psql >nul 2>nul
IF %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到 psql 命令
    echo 请确保 PostgreSQL 已安装并添加到 PATH
    echo 或者设置 PSQL_PATH 环境变量
    exit /b 1
)

REM 检查数据库连接
echo [检查] 连接数据库...
psql -h %DB_HOST% -p %DB_PORT% -d %DB_NAME% -c "SELECT 1" >nul 2>nul
IF %ERRORLEVEL% NEQ 0 (
    echo [错误] 无法连接到数据库
    echo 主机：%DB_HOST%:%DB_PORT%
    echo 数据库：%DB_NAME%
    echo 用户：%DB_USER%
    echo.
    echo 请检查：
    echo   1. PostgreSQL 服务是否运行
    echo   2. 数据库是否存在
    echo   3. 用户名密码是否正确
    exit /b 1
)
echo [成功] 数据库连接成功
echo.

REM 获取脚本所在目录
SET SCRIPT_DIR=%~dp0

REM 创建迁移历史表
echo [检查] 迁移历史表...
psql -h %DB_HOST% -p %DB_PORT% -d %DB_NAME% -f "%SCRIPT_DIR%000_create_schema_migrations_table.sql" >nul 2>nul

REM 获取已应用的版本
echo [检查] 已应用的迁移...
FOR /F "tokens=*" %%i IN ('psql -h %DB_HOST% -p %DB_PORT% -d %DB_NAME% -t -c "SELECT version FROM schema_migrations ORDER BY version;" 2^>nul') DO (
    SET APPLIED=!APPLIED! %%i
)
echo 已应用：%APPLIED%
echo.

REM 应用迁移
echo [开始] 应用迁移...
echo.

FOR %%F IN ("%SCRIPT_DIR%[0-9][0-9][0-9]_*.sql") DO (
    SET filename=%%~nxF
    REM 跳过回滚脚本
    echo !filename! | findstr /R "^rollback_" >nul
    IF !ERRORLEVEL! EQU 0 (
        echo [跳过] !filename! ^(回滚脚本^)
        GOTO :continue
    )
    
    REM 跳过历史表创建脚本（已执行）
    echo !filename! | findstr /R "^000_" >nul
    IF !ERRORLEVEL! EQU 0 (
        echo [跳过] !filename! ^(已执行^)
        GOTO :continue
    )
    
    echo [应用] !filename!
    psql -h %DB_HOST% -p %DB_PORT% -d %DB_NAME% -f "%%F" >nul 2>nul
    IF !ERRORLEVEL! EQU 0 (
        echo [成功] !filename!
    ) ELSE (
        echo [失败] !filename!
        echo 迁移中止！请检查错误信息。
        exit /b 1
    )
    echo.
    
    :continue
)

echo ========================================
echo    迁移完成！
echo ========================================
echo.

REM 显示最终状态
echo [状态] 当前迁移版本：
psql -h %DB_HOST% -p %DB_PORT% -d %DB_NAME% -c "SELECT version, applied_at FROM schema_migrations ORDER BY version;"

ENDLOCAL
