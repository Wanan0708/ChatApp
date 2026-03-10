# ESChat

ESChat 是一个 Qt/QML 桌面客户端，后端使用 Node.js + PostgreSQL，桌面端与网站端共用同一套服务接口。

## 目录

- src: Qt/C++ 客户端逻辑
- qml: Qt Quick 页面与组件
- server: Node.js API 与 WebSocket 服务
- web: 官网与下载页静态资源
- docs: 运行、使用、部署文档
- scripts: 本地联调启动脚本

## 快速开始

### 1. 初始化数据库

项目当前以后端 SQL 为准：

```bash
createdb -U postgres chatapp
psql -U postgres -d chatapp -f server/init_db.sql
```

如需执行增量迁移，可运行：

```bat
server\migrations\run_migrations.bat
```

如果要连同基础表一起初始化，可直接运行：

```powershell
./scripts/init-local-db.ps1
```

### 2. 启动后端

```bash
cd server
npm install
npm start
```

默认监听：

- HTTP: [http://127.0.0.1:8080](http://127.0.0.1:8080)
- WebSocket: ws://127.0.0.1:8080

### 3. 启动客户端

Windows 本地联调建议直接使用：

```powershell
./scripts/start-local-client.ps1
```

如果要顺手启动后端，也可以使用：

```powershell
./scripts/start-local-server.ps1
```

### 4. 访问网站

后端启动后可直接访问：

- [首页](http://127.0.0.1:8080/)
- [下载页](http://127.0.0.1:8080/download.html)
- [关于页](http://127.0.0.1:8080/about.html)

## 文档入口

- 使用说明：docs/使用指南.md
- 运行与部署：docs/运行部署指南.md
- 数据库说明：database/README_DATABASE.md
- 迁移说明：server/migrations/README_MIGRATIONS.md
- 网站部署：web/DEPLOY.md

## 当前运行约定

- 本地数据库名默认使用 chatapp
- server/.env 是当前后端配置来源
- 发布版 bin/ChatApp.exe 默认会走 Release 配置；做本地联调时应通过环境变量覆盖后端地址

## 常见问题

### 客户端请求落到远端

优先使用 scripts/start-local-client.ps1 启动。该脚本会设置本地联调所需的后端环境变量。

### 后端启动失败

先检查依赖和数据库：

```bash
cd server
npm install
```

再确认：

- PostgreSQL 服务已经启动
- server/.env 中的数据库配置可连通
- chatapp 数据库已执行过 server/init_db.sql
