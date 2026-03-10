Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent $scriptDir
$clientExe = Join-Path $projectRoot "bin\ChatApp.exe"

if (-not (Test-Path $clientExe)) {
    throw "未找到客户端可执行文件: $clientExe"
}

$env:CHATAPP_BACKEND_HOST = "127.0.0.1"
$env:CHATAPP_BACKEND_PORT = "8080"
$env:CHATAPP_API_SCHEME = "http"
$env:CHATAPP_WS_SCHEME = "ws"

Write-Host "[ESChat] 使用本地后端启动客户端"
Write-Host "[ESChat] CHATAPP_BACKEND_HOST=$env:CHATAPP_BACKEND_HOST"
Write-Host "[ESChat] CHATAPP_BACKEND_PORT=$env:CHATAPP_BACKEND_PORT"

Start-Process -FilePath $clientExe -WorkingDirectory (Split-Path $clientExe -Parent)