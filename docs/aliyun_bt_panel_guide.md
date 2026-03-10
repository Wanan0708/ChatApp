# ESChat 阿里云部署指南

本文档面向当前仓库版本，适用于阿里云 ECS 部署 Node.js + PostgreSQL + Nginx。

如果你使用宝塔面板，核心流程也一样：数据库先执行基础建表和迁移，Node 服务跑 server/index.js，Nginx 反向代理到 127.0.0.1:8080。

## 适用范围

- 阿里云 ECS Linux 服务器
- Ubuntu 22.04 / Debian 12 / CentOS 7+ 均可参考
- PostgreSQL 本机部署或阿里云 RDS PostgreSQL 均可
- Nginx 对外提供 HTTPS 和 WebSocket 反向代理

## 部署结构

推荐目录：

```text
/opt/eschat/
├── server/
├── web/
├── qml/
├── image/
└── 其他仓库文件
```

当前服务约定：

- 后端入口：server/index.js
- 监听端口：8080
- Nginx 反向代理目标：127.0.0.1:8080
- 网站静态资源由 Node 服务直接读取 web 目录提供

## 1. 准备服务器环境

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install -y nginx postgresql postgresql-client
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
sudo apt install -y nodejs
sudo npm install -g pm2
```

### CentOS

```bash
sudo yum install -y nginx postgresql postgresql-server
curl -fsSL https://rpm.nodesource.com/setup_20.x | sudo bash -
sudo yum install -y nodejs
sudo npm install -g pm2
```

确认版本：

```bash
node -v
npm -v
psql --version
nginx -v
pm2 -v
```

## 2. 上传代码

可以任选一种方式：

### 方式一：git clone

```bash
sudo mkdir -p /opt/eschat
sudo chown -R $USER:$USER /opt/eschat
cd /opt/eschat
git clone 你的仓库地址 .
```

### 方式二：本地打包后上传

将当前仓库上传到服务器，例如解压到：

```text
/opt/eschat
```

## 3. 安装后端依赖

```bash
cd /opt/eschat/server
npm install
```

## 4. 配置数据库

### 本机 PostgreSQL

先创建数据库：

```bash
sudo -u postgres createdb chatapp
```

初始化基础表：

```bash
cd /opt/eschat
sudo -u postgres psql -d chatapp -f server/init_db.sql
```

执行增量迁移：

```bash
cd /opt/eschat/server
export CHATAPP_DB_HOST=localhost
export CHATAPP_DB_PORT=5432
export CHATAPP_DB_NAME=chatapp
export CHATAPP_DB_USER=postgres
./migrations/run_migrations.sh
```

如果 postgres 用户需要密码认证，再额外设置：

```bash
export PGPASSWORD=你的数据库密码
export CHATAPP_DB_PASSWORD=你的数据库密码
```

### 阿里云 RDS PostgreSQL

如果数据库在 RDS，把环境变量换成 RDS 地址：

```bash
cd /opt/eschat/server
export CHATAPP_DB_HOST=你的RDS地址
export CHATAPP_DB_PORT=5432
export CHATAPP_DB_NAME=chatapp
export CHATAPP_DB_USER=你的数据库用户
export PGPASSWORD=你的数据库密码
export CHATAPP_DB_PASSWORD=你的数据库密码
psql -h "$CHATAPP_DB_HOST" -p "$CHATAPP_DB_PORT" -U "$CHATAPP_DB_USER" -d "$CHATAPP_DB_NAME" -f ../server/init_db.sql
./migrations/run_migrations.sh
```

### 迁移说明

当前项目数据库变更规则是：

- server/init_db.sql 只负责基础建表
- server/migrations/*.sql 负责增量升级
- server/index.js 启动时只校验 schema，不再自动执行 ALTER TABLE

所以服务启动前，基础建表和迁移必须先执行完成。

## 5. 配置 server/.env

在服务器创建或更新：

```text
/opt/eschat/server/.env
```

最少配置如下：

```env
PORT=8080
DB_HOST=localhost
DB_PORT=5432
DB_NAME=chatapp
DB_USER=postgres
DB_PASSWORD=123456
JWT_SECRET=replace_with_a_long_random_secret
```

如果你连接的是 RDS，请把 DB_HOST、DB_USER、DB_PASSWORD 改成实际值。

## 6. 启动 Node 服务

### 方式一：pm2

```bash
cd /opt/eschat/server
pm2 start index.js --name eschat-server
pm2 save
pm2 startup
```

常用命令：

```bash
pm2 status
pm2 logs eschat-server --lines 100
pm2 restart eschat-server
pm2 stop eschat-server
```

### 方式二：systemd

创建服务文件：

```text
/etc/systemd/system/eschat.service
```

内容示例：

```ini
[Unit]
Description=ESChat Node Server
After=network.target

[Service]
Type=simple
WorkingDirectory=/opt/eschat/server
ExecStart=/usr/bin/node /opt/eschat/server/index.js
Restart=always
RestartSec=5
Environment=NODE_ENV=production

[Install]
WantedBy=multi-user.target
```

启用并启动：

```bash
sudo systemctl daemon-reload
sudo systemctl enable eschat
sudo systemctl start eschat
sudo systemctl status eschat
```

## 7. 配置 Nginx

Nginx 需要把所有流量转发到 Node 服务，并保留 WebSocket 升级头。

示例配置：

```nginx
server {
    listen 80;
    server_name 你的域名或公网IP;

    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_read_timeout 86400;
        proxy_send_timeout 86400;
    }
}
```

启用配置：

```bash
sudo nginx -t
sudo systemctl restart nginx
```

如果要上 HTTPS，建议配好证书后把 80 跳转到 443，并保留同样的 proxy 配置。

## 8. 阿里云安全组与防火墙

至少放行：

- 80
- 443
- 22

通常不建议直接暴露：

- 8080
- 5432

Node 和 PostgreSQL 应尽量只监听本机或内网，由 Nginx 对外提供访问入口。

## 9. 部署后验证

### 检查服务日志

```bash
pm2 logs eschat-server --lines 100
```

或：

```bash
sudo journalctl -u eschat -n 100 --no-pager
```

### 检查数据库迁移状态

```bash
psql -h 127.0.0.1 -p 5432 -U postgres -d chatapp -c "SELECT version, applied_at FROM schema_migrations ORDER BY version;"
```

### 检查网站和 API

```bash
curl http://127.0.0.1:8080/
curl http://127.0.0.1:8080/download.html
curl http://127.0.0.1:8080/api/conversations
```

未带认证访问 API 返回未授权是正常现象，只要服务有响应即可。

## 10. 更新发布流程

服务已上线后，推荐按下面顺序更新：

```bash
cd /opt/eschat
git pull
cd server
npm install
export CHATAPP_DB_HOST=localhost
export CHATAPP_DB_PORT=5432
export CHATAPP_DB_NAME=chatapp
export CHATAPP_DB_USER=postgres
export PGPASSWORD=你的数据库密码
./migrations/run_migrations.sh
pm2 restart eschat-server
```

如果是 systemd：

```bash
sudo systemctl restart eschat
```

## 常见问题

### 服务启动时报数据库 schema 过旧

说明数据库没有执行完迁移。先运行：

```bash
cd /opt/eschat/server
./migrations/run_migrations.sh
```

### 端口 8080 被占用

检查占用进程：

```bash
sudo ss -ltnp | grep 8080
```

### 上传头像后文件访问不到

确认 Node 服务可读取 web/uploads/avatars 目录，并且 Nginx 没有绕过 Node 直接指向错误站点根目录。

## 相关文档

- docs/运行部署指南.md
- server/migrations/README_MIGRATIONS.md
- database/README_DATABASE.md
- web/DEPLOY.md
