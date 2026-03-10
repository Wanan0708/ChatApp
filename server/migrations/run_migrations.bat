@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "DB_HOST=%CHATAPP_DB_HOST%"
if "%DB_HOST%"=="" set "DB_HOST=localhost"

set "DB_PORT=%CHATAPP_DB_PORT%"
if "%DB_PORT%"=="" set "DB_PORT=5432"

set "DB_NAME=%~1"
if "%DB_NAME%"=="" set "DB_NAME=%CHATAPP_DB_NAME%"
if "%DB_NAME%"=="" set "DB_NAME=chatapp"

set "DB_USER=%CHATAPP_DB_USER%"
if "%DB_USER%"=="" set "DB_USER=postgres"

set "PGUSER=%DB_USER%"
set "PGPASSWORD=%CHATAPP_DB_PASSWORD%"
set "SCRIPT_DIR=%~dp0"

echo ========================================
echo    ChatApp Database Migration Tool
echo ========================================
echo.

where psql >nul 2>nul
if errorlevel 1 (
    echo [ERROR] psql command not found
    echo Ensure PostgreSQL is installed and added to PATH
    exit /b 1
)

echo [CHECK] Connecting to database...
psql -h %DB_HOST% -p %DB_PORT% -U %DB_USER% -d %DB_NAME% -c "SELECT 1" >nul 2>nul
if errorlevel 1 (
    echo [ERROR] Unable to connect to database
    echo Host: %DB_HOST%:%DB_PORT%
    echo Database: %DB_NAME%
    echo User: %DB_USER%
    exit /b 1
)
echo [OK] Database connection successful
echo.

echo [CHECK] Ensuring schema_migrations exists...
psql -h %DB_HOST% -p %DB_PORT% -U %DB_USER% -d %DB_NAME% -f "%SCRIPT_DIR%000_create_schema_migrations_table.sql" >nul 2>nul

echo [START] Applying migrations...
echo.

pushd "%SCRIPT_DIR%"
for /f "delims=" %%F in ('dir /b /on *.sql 2^>nul') do (
    set "FILE_NAME=%%F"
    set "VERSION=%%F"
    for /f "tokens=1 delims=_" %%V in ("%%F") do set "VERSION=%%V"

    echo !VERSION!| findstr /R "^[0-9][0-9][0-9]$" >nul
    if errorlevel 1 (
        echo [SKIP] !FILE_NAME! ^(not a numbered migration^)
    ) else if /i "!VERSION!"=="000" (
        echo [SKIP] !FILE_NAME! ^(bootstrap script^)
    ) else (
        set "APPLIED_VERSION="
        for /f "usebackq tokens=* delims=" %%A in (`psql -h %DB_HOST% -p %DB_PORT% -U %DB_USER% -d %DB_NAME% -t -A -c "SELECT version FROM schema_migrations WHERE version = '!VERSION!';" 2^>nul`) do (
            set "APPLIED_VERSION=%%A"
        )

        if /i "!APPLIED_VERSION!"=="!VERSION!" (
            echo [SKIP] !FILE_NAME! ^(already applied^)
        ) else (
            echo [APPLY] !FILE_NAME!
            psql -h %DB_HOST% -p %DB_PORT% -U %DB_USER% -d %DB_NAME% -f "%SCRIPT_DIR%!FILE_NAME!"
            if errorlevel 1 (
                echo [FAIL] !FILE_NAME!
                exit /b 1
            )
            echo [OK] !FILE_NAME!
            echo.
        )
    )
)
popd

echo ========================================
echo    Migration completed
echo ========================================
echo.
echo [STATE] Current migration versions:
psql -h %DB_HOST% -p %DB_PORT% -U %DB_USER% -d %DB_NAME% -c "SELECT version, applied_at FROM schema_migrations ORDER BY version;"

endlocal
