# XWChat 阿里云服务器部署操作文档

> 文档版本：v1.0  
> 更新日期：2026 年 3 月 6 日  
> 适用系统：Ubuntu 22.04 LTS / CentOS 7.9+

---

## 📋 目录

1. [部署前准备](#1-部署前准备)
2. [服务器环境配置](#2-服务器环境配置)
3. [数据库部署](#3-数据库部署)
4. [后端服务部署](#4-后端服务部署)
5. [Nginx 反向代理与 SSL 配置](#5-nginx 反向代理与 ssl 配置)
6. [客户端打包与发布](#6-客户端打包与发布)
7. [验证与测试](#7-验证与测试)
8. [常见问题排查](#8-常见问题排查)

---

## 1. 部署前准备

### 1.1 阿里云控制台配置

#### 安全组规则
登录 [阿里云控制台](https://ecs.console.aliyun.com/) → 网络安全 → 安全组，添加以下规则：

| 端口 | 协议 | 授权对象 | 说明 |
|------|------|----------|------|
| 22 | TCP | 0.0.0.0/0 | SSH 远程连接 |
| 80 | TCP | 0.0.0.0/0 | HTTP 访问 |
| 443 | TCP | 0.0.0.0/0 | HTTPS/WSS 访问 |
| 8080 | TCP | 127.0.0.1/32 | 后端服务 (仅本地) |
| 5432 | TCP | 127.0.0.1/32 | PostgreSQL (仅本地) |

> ⚠️ **安全警告**: 数据库端口 5432 切勿对公网开放！

### 1.2 本地文件准备

需要上传到服务器的文件清单：

```
server/
├── index.js              # Node.js 后端主程序
├── package.json          # 依赖配置
├── package-lock.json     # 依赖锁定文件
├── .env                  # 环境变量配置 (需修改)
└── init_db.sql           # 数据库初始化脚本
```

---

## 2. 服务器环境配置

### 2.1 连接服务器

```bash
ssh root@59.110.44.145
```

### 2.2 系统更新

**Ubuntu 系统:**
```bash
sudo apt update && sudo apt upgrade -y
```

**CentOS 系统:**
```bash
sudo yum update -y
```

### 2.3 安装基础工具

```bash
# Ubuntu
sudo apt install -y git curl wget vim net-tools

# CentOS
sudo yum install -y git curl wget vim net-tools
```

### 2.4 配置防火墙

**Ubuntu (UFW):**
```bash
sudo ufw enable
sudo ufw allow 22/tcp
sudo ufw allow 80/tcp
sudo ufw allow 443/tcp
sudo ufw status
```

**CentOS (Firewalld):**
```bash
sudo firewall-cmd --permanent --add-port=22/tcp
sudo firewall-cmd --permanent --add-port=80/tcp
sudo firewall-cmd --permanent --add-port=443/tcp
sudo firewall-cmd --reload
```

---

## 3. 数据库部署

### 3.1 安装 PostgreSQL

**Ubuntu:**
```bash
sudo apt install -y postgresql postgresql-contrib postgresql-client
```

**CentOS:**
```bash
sudo yum install -y https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm
sudo yum install -y postgresql15-server postgresql15
sudo /usr/pgsql-15/bin/postgresql-15-setup initdb
```

### 3.2 启动 PostgreSQL 服务

```bash
# Ubuntu
sudo systemctl start postgresql
sudo systemctl enable postgresql

# CentOS
sudo systemctl start postgresql-15
sudo systemctl enable postgresql-15
```

### 3.3 配置数据库用户

```bash
sudo -i -u postgres
psql
```

在 PostgreSQL 交互界面执行：

```sql
-- 创建数据库
CREATE DATABASE chatapp;

-- 设置用户密码 (请修改为强密码)
ALTER USER postgres WITH PASSWORD 'YourStrongPassword123!';

-- 授权
GRANT ALL PRIVILEGES ON DATABASE chatapp TO postgres;

-- 退出
\q
exit
```

### 3.4 配置远程访问 (可选，仅内网)

编辑 PostgreSQL 配置文件：

```bash
sudo vim /etc/postgresql/*/main/postgresql.conf
# 或 CentOS: sudo vim /var/lib/pgsql/15/data/postgresql.conf
```

修改：
```conf
listen_addresses = 'localhost'
```

编辑 `pg_hba.conf`:
```bash
sudo vim /etc/postgresql/*/main/pg_hba.conf
```

确保只有本地连接：
```conf
local   all             all                                     md5
host    all             all             127.0.0.1/32            md5
```

重启 PostgreSQL：
```bash
sudo systemctl restart postgresql
```

### 3.5 导入数据库架构

上传 `init_db.sql` 到服务器后执行：

```bash
psql -U postgres -d chatapp -f /path/to/init_db.sql
```

验证表是否创建成功：
```bash
psql -U postgres -d chatapp -c "\dt"
```

---

## 4. 后端服务部署

### 4.1 安装 Node.js

使用 NVM 安装（推荐）:

```bash
# 安装 NVM
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.5/install.sh | bash
source ~/.bashrc

# 安装 Node.js LTS 版本
nvm install 20
nvm use 20
nvm alias default 20

# 验证
node --version
npm --version
```

### 4.2 部署应用代码

```bash
# 创建应用目录
sudo mkdir -p /opt/xwchat
sudo chown $USER:$USER /opt/xwchat

# 上传 server 目录内容到 /opt/xwchat
# 可以使用 scp 或 FTP 工具上传
```

### 4.3 安装依赖

```bash
cd /opt/xwchat
npm install --production
```

### 4.4 配置环境变量

创建或编辑 `.env` 文件：

```bash
vim /opt/xwchat/.env
```

内容如下（**请修改密码和 JWT 密钥**）:

```env
PORT=8080
DB_HOST=localhost
DB_PORT=5432
DB_NAME=chatapp
DB_USER=postgres
DB_PASSWORD=YourStrongPassword123!
JWT_SECRET=your_random_secret_key_2026_change_this
```

### 4.5 安装 PM2 进程管理

```bash
npm install -g pm2
```

### 4.6 启动服务

```bash
cd /opt/xwchat

# 启动应用
pm2 start index.js --name xwchat-server

# 设置开机自启
pm2 save
pm2 startup

# 查看状态
pm2 status
pm2 logs xwchat-server
```

### 4.7 验证后端服务

```bash
# 本地测试
curl http://127.0.0.1:8080/api/health

# 查看日志
pm2 logs xwchat-server --lines 50
```

---

## 5. Nginx 反向代理与 SSL 配置

### 5.1 安装 Nginx

**Ubuntu:**
```bash
sudo apt install -y nginx
```

**CentOS:**
```bash
sudo yum install -y epel-release
sudo yum install -y nginx
```

启动 Nginx：
```bash
sudo systemctl start nginx
sudo systemctl enable nginx
```

### 5.2 申请 SSL 证书

#### 方式一：使用 Certbot (推荐)

```bash
# 安装 Certbot
sudo apt install -y certbot python3-certbot-nginx  # Ubuntu
# 或
sudo yum install -y certbot python3-certbot-nginx  # CentOS

# 申请证书 (需要域名)
sudo certbot --nginx -d your-domain.com
```

#### 方式二：阿里云 SSL 证书

1. 登录 [阿里云 SSL 证书控制台](https://yundun.console.aliyun.com/?p=cas)
2. 申请免费证书或购买商业证书
3. 下载 Nginx 格式的证书文件
4. 上传到服务器 `/etc/nginx/ssl/` 目录

### 5.3 配置 Nginx 反向代理

创建配置文件：

```bash
sudo vim /etc/nginx/sites-available/xwchat
# 或 CentOS: sudo vim /etc/nginx/conf.d/xwchat.conf
```

配置内容：

```nginx
# HTTP 重定向到 HTTPS
server {
    listen 80;
    server_name 59.110.44.145 your-domain.com;
    
    location / {
        return 301 https://$server_name$request_uri;
    }
}

# HTTPS 服务器
server {
    listen 443 ssl http2;
    server_name 59.110.44.145 your-domain.com;

    # SSL 证书配置 (Certbot 自动填充或手动指定)
    ssl_certificate /etc/letsencrypt/live/your-domain.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/your-domain.com/privkey.pem;
    
    # SSL 优化配置
    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers HIGH:!aNULL:!MD5;
    ssl_prefer_server_ciphers on;
    ssl_session_cache shared:SSL:10m;
    ssl_session_timeout 10m;

    # WebSocket 反向代理
    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_cache_bypass $http_upgrade;
        
        # WebSocket 长连接超时设置
        proxy_read_timeout 86400s;
        proxy_send_timeout 86400s;
    }

    # API 接口代理
    location /api {
        proxy_pass http://127.0.0.1:8080;
        proxy_http_version 1.1;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }
}
```

启用配置并重启 Nginx：

```bash
# Ubuntu
sudo ln -s /etc/nginx/sites-available/xwchat /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl restart nginx

# CentOS
sudo nginx -t
sudo systemctl restart nginx
```

### 5.4 验证 Nginx 配置

```bash
# 检查配置语法
sudo nginx -t

# 查看 Nginx 状态
sudo systemctl status nginx

# 测试 HTTPS
curl -k https://59.110.44.145/api/health
```

---

## 6. 客户端打包与发布

### 6.1 修改客户端配置

客户端通过环境变量配置后端地址，无需修改代码。部署包可附带启动脚本：

**Windows 启动脚本 (start.bat):**
```batch
@echo off
set CHATAPP_BACKEND_HOST=59.110.44.145
set CHATAPP_BACKEND_PORT=443
set CHATAPP_API_SCHEME=https
set CHATAPP_WS_SCHEME=wss
start ChatApp.exe
```

**Linux 启动脚本 (start.sh):**
```bash
#!/bin/bash
export CHATAPP_BACKEND_HOST=59.110.44.145
export CHATAPP_BACKEND_PORT=443
export CHATAPP_API_SCHEME=https
export CHATAPP_WS_SCHEME=wss
./ChatApp
```

### 6.2 Windows 客户端打包

在 Windows 开发机上执行：

```cmd
# 1. Release 模式编译 Qt 项目
# 在 Qt Creator 中选择 Release 配置编译

# 2. 进入输出目录
cd E:\Project\QtProject\ChatApp\bin\release

# 3. 部署 Qt 依赖
windeployqt ChatApp.exe --qmldir E:\Project\QtProject\ChatApp\qml

# 4. 复制 PostgreSQL 驱动
# 从 PostgreSQL 安装目录复制以下文件到 ChatApp.exe 同目录：
# - libpq.dll
# - libintl-9.dll (版本号可能不同)
# - libiconv-2.dll
# - libcrypto-1_1-x64.dll
# - libssl-1_1-x64.dll

# 5. 确保 sqldrivers 目录存在并包含 qsqlpsql.dll
```

### 6.3 Linux 客户端打包 (可选)

```bash
# 编译
qmake ChatApp.pro CONFIG+=release
make -j4

# 部署依赖
linuxdeployqt --appdir AppDir ChatApp
```

---

## 7. 验证与测试

### 7.1 服务健康检查

```bash
# 检查 PostgreSQL
sudo systemctl status postgresql
psql -U postgres -d chatapp -c "SELECT version();"

# 检查 Node.js 服务
pm2 status
pm2 logs xwchat-server --lines 20

# 检查 Nginx
sudo systemctl status nginx
sudo tail -f /var/log/nginx/access.log
```

### 7.2 网络连通性测试

```bash
# 测试本地端口
curl http://127.0.0.1:8080/api/health

# 测试 HTTPS
curl -k https://59.110.44.145/api/health

# 测试 WebSocket (需要 wscat 工具)
wscat -c wss://59.110.44.145/ws
```

### 7.3 客户端功能测试

1. **登录测试**: 使用测试账号登录
2. **消息发送**: 发送文本消息验证 WebSocket 连接
3. **好友功能**: 测试好友添加和请求
4. **断线重连**: 模拟网络中断后验证自动重连

### 7.4 监控配置

```bash
# 查看 PM2 监控
pm2 monit

# 查看 Nginx 实时日志
sudo tail -f /var/log/nginx/access.log
sudo tail -f /var/log/nginx/error.log

# 查看应用日志
pm2 logs xwchat-server
```

---

## 8. 常见问题排查

### 8.1 数据库连接失败

**症状**: 客户端登录提示数据库连接错误

**排查步骤**:
```bash
# 1. 检查 PostgreSQL 状态
sudo systemctl status postgresql

# 2. 检查数据库用户密码
psql -U postgres -d chatapp -c "SELECT usename FROM pg_user;"

# 3. 检查后端日志
pm2 logs xwchat-server | grep -i error

# 4. 验证 .env 配置
cat /opt/xwchat/.env
```

**解决方案**: 确保 `.env` 中的数据库密码与 PostgreSQL 用户密码一致

### 8.2 WebSocket 连接失败

**症状**: 客户端显示"连接服务器失败"

**排查步骤**:
```bash
# 1. 检查 Nginx 配置
sudo nginx -t

# 2. 检查 WebSocket 代理配置
sudo grep -A 10 "proxy_set_header Upgrade" /etc/nginx/sites-available/xwchat

# 3. 检查防火墙
sudo ufw status  # Ubuntu
sudo firewall-cmd --list-all  # CentOS

# 4. 测试本地 WebSocket
curl -i -N -H "Connection: Upgrade" -H "Upgrade: websocket" http://127.0.0.1:8080/ws
```

### 8.3 SSL 证书问题

**症状**: 浏览器提示证书不安全

**解决方案**:
```bash
# 1. 检查证书有效期
sudo certbot certificates

# 2. 续期证书
sudo certbot renew

# 3. 重启 Nginx
sudo systemctl restart nginx
```

### 8.4 服务无法启动

**症状**: PM2 显示服务为 error 状态

**排查步骤**:
```bash
# 查看错误日志
pm2 logs xwchat-server --err

# 检查端口占用
sudo netstat -tlnp | grep 8080

# 检查 Node 版本
node --version

# 重新安装依赖
cd /opt/xwchat
rm -rf node_modules
npm install --production
pm2 restart xwchat-server
```

### 8.5 性能优化建议

```nginx
# Nginx 优化 (/etc/nginx/nginx.conf)
worker_processes auto;
worker_rlimit_nofile 65535;

events {
    use epoll;
    worker_connections 65535;
    multi_accept on;
}

http {
    # 开启 gzip 压缩
    gzip on;
    gzip_types text/plain application/json;
    
    # 连接优化
    keepalive_timeout 65;
    keepalive_requests 100;
}
```

---

## 📞 技术支持

如遇到问题，请收集以下信息：

1. 系统日志：`sudo journalctl -xe`
2. Nginx 日志：`/var/log/nginx/error.log`
3. 应用日志：`pm2 logs xwchat-server`
4. 数据库日志：`/var/log/postgresql/postgresql-*.log`

---

## 🔐 安全核查清单

部署完成后，请确认以下安全配置：

- [ ] PostgreSQL 仅允许本地连接
- [ ] 防火墙已启用，仅开放必要端口
- [ ] JWT_SECRET 已修改为强随机字符串
- [ ] 数据库密码已修改为强密码
- [ ] SSL 证书已正确配置并启用强制 HTTPS
- [ ] 定期备份数据库数据
- [ ] 配置日志轮转，避免磁盘占满

---

**文档结束**
