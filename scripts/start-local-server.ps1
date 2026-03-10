Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent $scriptDir
$serverDir = Join-Path $projectRoot "server"

if (-not (Test-Path $serverDir)) {
    throw "未找到 server 目录: $serverDir"
}

Push-Location $serverDir
try {
    if (-not (Test-Path (Join-Path $serverDir "node_modules"))) {
        Write-Host "[ESChat] 安装后端依赖..."
        npm install
    }

    Write-Host "[ESChat] 启动本地后端 http://127.0.0.1:8080"
    npm start
}
finally {
    Pop-Location
}