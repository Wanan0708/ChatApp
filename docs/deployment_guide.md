# XWChat (ChatApp) 部署上线详细教程

本教程涵盖了从本地开发环境到阿里云服务器（Ubuntu/CentOS）的完整部署流程，包括数据库迁移、后端服务部署、SSL/WSS 安全配置以及客户端打包。

## 1. 架构说明 (Architecture)

本应用采用“重客户端 + 轻服务端”的模式，目前的实现中：
*   **本地/客户端**: Qt 应用直接通过 `QPSQL` 驱动连接 PostgreSQL 数据库（用于数据查询与验证）并使用 WebSocket 与服务器通信。
*   **服务器端**: Node.js 后端连接同一个（或同步的）PostgreSQL 数据库，处理消息转发、心跳和在线状态。

> [!WARNING]
> **安全警告**: 让客户端直接连接公网数据库存在巨大安全风险。建议未来将所有数据库操作改为通过 Node.js 的 HTTP/WebSocket 接口完成。

## 2. 准备工作

### 2.1 服务器环境 (阿里云)
*   **操作系统**: 推荐使用 Ubuntu 22.04 LTS 或 CentOS 7.9/8。
*   **开放端口**: 在阿里云控制台安全组中开放以下端口：
    *   `80` (HTTP)
    *   `443` (HTTPS/WSS)
    *   `8080` (后端服务端口，若使用 Nginx 转发则可仅对内网开放)
    *   `5432` (PostgreSQL，建议仅允许本地或特定 IP 访问)

### 2.2 域名准备
*   域名: `59.110.44.145`
*   解析: 将 A 记录指向服务器 IP 地址。

---

## 3. 数据库部署 (Server-side)

### 3.1 安装 PostgreSQL
在 Ubuntu 上：
```bash
sudo apt update
sudo apt install postgresql postgresql-contrib
```

### 3.2 配置数据库
登录 PostgreSQL 并创建数据库和用户：
```bash
sudo -i -u postgres
psql

# 在 psql 交互界面中：
CREATE DATABASE chatapp;
CREATE USER postgres WITH PASSWORD '123456'; # 匹配 .env 和代码默认值
GRANT ALL PRIVILEGES ON DATABASE chatapp TO postgres;
\q
exit
```

### 3.3 导入架构
将本地的 `server/init_db.sql` 上传到服务器并执行：
```bash
psql -U postgres -d chatapp -f /path/to/init_db.sql
```

> [!IMPORTANT]
> **本地数据库**: 您本地也需要安装 PostgreSQL 15+ 并运行 `init_db.sql` 以便开发调试。

---

## 4. 后端服务部署 (Node.js)

### 4.1 安装 Node.js
推荐使用 NVM 安装：
```bash
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.5/install.sh | bash
source ~/.bashrc
nvm install 20
```

### 4.2 部署代码
1.  克隆代码或上传 `server` 目录。
2.  安装依赖：
    ```bash
    cd server
    npm install
    ```
3.  **配置环境变量**: 创建 `.env` 文件：
    ```env
    PORT=8080
    DB_HOST=localhost
    DB_PORT=5432
    DB_NAME=chatapp
    DB_USER=postgres
    DB_PASSWORD=123456
    JWT_SECRET=您的随机长字符串
    ```

### 4.3 使用 PM2 管理进程
```bash
npm install -g pm2
pm2 start index.js --name "xwchat-server"
pm2 save
pm2 startup
```

---

## 5. 网络安全与 SSL (Nginx & WSS)

由于客户端需要使用 `ws://` 指向生产环境，可以配置 Nginx 作为反向代理。

### 5.1 安装 Nginx
```bash
sudo apt install nginx
```

### 5.2 配置 Nginx 转发 WebSocket
修改 `/etc/nginx/sites-available/default` (或相应配置文件)：

```nginx
server {
    listen 80;
    server_name 59.110.44.145;

    location / {
        proxy_pass http://59.110.44.145:8080;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
        proxy_set_header Host $host;
        proxy_cache_bypass $http_upgrade;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }

    listen 443 ssl; # 由 certbot 自动管理
    # ... certbot 生成的 ssl 配置 ...
}
```
重启 Nginx: `sudo systemctl restart nginx`

---

## 6. 客户端配置与打包 (Qt)

### 6.1 修改配置 (重要)
客户端通过环境变量加载后端地址配置（API 和 WebSocket），不再需要修改 `chatservice.cpp` 里的硬编码地址。

**代码位置**: `src/database/databaseconfig.cpp`
*   **Debug 模式默认主机**: `127.0.0.1`
*   **Release 模式默认主机**: `59.110.44.145`
*   **默认端口**: `8080`

可用环境变量（任选）：
```cmd
set CHATAPP_BACKEND_HOST=59.110.44.145
set CHATAPP_BACKEND_PORT=8080
set CHATAPP_API_SCHEME=https
set CHATAPP_WS_SCHEME=wss
REM 可选：直接覆盖完整地址
set CHATAPP_API_BASE_URL=https://chat.example.com/api
set CHATAPP_WS_URL=wss://chat.example.com/ws
ChatApp.exe
```

说明：
*   如果设置了 `CHATAPP_API_BASE_URL`，客户端会优先使用它作为 API 基础地址。
*   如果设置了 `CHATAPP_WS_URL`，客户端会优先使用它作为 WebSocket 地址。
*   未设置时，客户端会自动拼接 `http(s)://host:port/api` 与 `ws(s)://host:port`。
*   客户端内置自动重连（指数退避，最大 60 秒）和消息断线队列（最多 200 条）。
*   在 Release 模式下如果仍使用 `http/ws`，客户端会输出安全告警日志，建议切换到 `https/wss`。

### 6.2 Windows 打包
1.  在 Qt Creator 中以 **Release** 模式编译项目。
2.  打开 `Qt 终端` 并运行 `windeployqt`:
    ```cmd
    windeployqt ChatApp.exe --qmldir E:\Project\QtProject\ChatApp\qml
    ```
3.  **手动补充驱动**: 因为程序直连数据库，必须将 PostgreSQL 的动态库（`libpq.dll`, `libintl-9.dll` 等，位于 PostgreSQL 安装目录的 `bin` 下）拷贝到 `ChatApp.exe` 所在目录。
4.  同时也需要确保 `sqldrivers/qsqlpsql.dll`存在于打包目录。

---

## 7. 验证流程

1.  **后端存活**: 访问 `https://59.110.44.145` (如果后端有简单的根路由响应)。
2.  **WS 连接**: 使用在线 WebSocket 测试工具连接您配置后的地址（例如 `wss://chat.example.com/ws`）。
3.  **全链路**: 启动打包后的 `.exe`，尝试注册、登录、发送消息。

---

> [!IMPORTANT]
> **生产环境安全核查清单**:
> - [ ] 禁用 PostgreSQL 的远程 root 登录。
> - [ ] 将 `JWT_SECRET` 修改为生产用强核。
> - [ ] 确认 Nginx 的 `client_max_body_size` 足够处理头像上传。
> - [ ] 开启服务器防火墙 (ufw/firewalld)，仅保留必要端口。
