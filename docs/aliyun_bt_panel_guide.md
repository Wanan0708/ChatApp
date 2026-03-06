# XWChat 宝塔面板部署指南（阿里云专用）

> 适用对象：使用宝塔面板管理阿里云服务器的用户  
> 宝塔版本要求：v8.0+  
> 更新时间：2026 年 3 月 6 日

---

## 📋 目录

1. [宝塔面板安装](#1-宝塔面板安装)
2. [环境软件安装](#2-环境软件安装)
3. [数据库配置](#3-数据库配置)
4. [后端服务部署](#4-后端服务部署)
5. [网站与 SSL 配置](#5-网站与 ssl 配置)
6. [Nginx 反向代理配置](#6-nginx 反向代理配置)
7. [客户端配置](#7-客户端配置)
8. [常见问题排查](#8-常见问题排查)

---

## 1. 宝塔面板安装

### 1.1 安装宝塔面板

通过 SSH 连接服务器后执行：

```bash
# Ubuntu/Debian
wget -O install.sh https://download.bt.cn/install/install-ubuntu_6.0.sh && bash install.sh

# CentOS
wget -O install.sh https://download.bt.cn/install/install_6.0.sh && bash install.sh
```

安装完成后，记录面板登录地址、用户名和密码。

### 1.2 登录面板

浏览器访问面板地址，登录后建议：
1. 修改默认用户名和密码
2. 绑定宝塔账号（方便远程管理）
3. 安装推荐插件套件

---

## 2. 环境软件安装

### 2.1 安装软件商店

在宝塔左侧菜单点击 **「软件商店」**，安装以下软件：

| 软件 | 版本建议 | 说明 |
|------|----------|------|
| Nginx | 1.24+ | Web 服务器 |
| MySQL/PostgreSQL | PostgreSQL 15 | 数据库（二选一） |
| Node.js | 20.x | 后端运行环境 |
| PM2 管理器 | 最新版 | Node 进程管理 |

### 2.2 安装 PostgreSQL

1. 点击 **「数据库」** → 右侧 **「PostgreSQL」** → 点击安装
2. 安装完成后，点击 **「设置」** 进行初始化
3. 设置 `postgres` 用户密码（请记录并保存）

### 2.3 安装 Nginx

1. 点击 **「软件商店」** → 找到 Nginx → 点击安装
2. 选择稳定版即可

### 2.4 安装 Node.js 版本管理器

1. 点击 **「Node 版本管理」** → 安装
2. 添加 Node.js 版本：点击 **「添加」** → 选择 **20.x LTS** → 安装

---

## 3. 数据库配置

### 3.1 创建数据库

1. 点击左侧 **「数据库」** 菜单
2. 点击顶部 **「PostgreSQL」** 标签切换
3. 点击 **「添加数据库」**

填写信息：
- **数据库名**: `chatapp`
- **用户名**: `postgres`
- **密码**: （设置强密码，记录到 `.env` 文件）
- **权限**: `所有权限`

### 3.2 导入数据表结构

1. 点击刚创建的数据库右侧的 **「管理」** 按钮
2. 进入 phpPgAdmin 管理界面
3. 点击顶部菜单 **「SQL」** 标签
4. 将本地 `server/init_db.sql` 文件内容粘贴到文本框
5. 点击 **「执行」** 按钮

验证导入结果：
```sql
SELECT tablename FROM pg_tables WHERE schemaname = 'public';
```

应该看到 `users`、`messages`、`conversations`、`friends` 等表。

### 3.3 配置数据库访问权限

> ⚠️ **重要**: 默认仅允许本地连接，无需修改。如需远程管理：

1. 点击 **「数据库」** → PostgreSQL **「设置」**
2. 在 `pg_hba.conf` 配置文件中，确保只有本地连接：
   ```conf
   host    all    all    127.0.0.1/32    md5
   ```
3. 点击 **「保存配置」**

---

## 4. 后端服务部署

### 4.1 上传项目文件

1. 点击左侧 **「文件」** 菜单
2. 进入 `/www/wwwroot/` 目录
3. 新建文件夹 `xwchat`
4. 点击 **「上传」** 按钮，上传以下文件：
   - `index.js`
   - `package.json`
   - `package-lock.json`
   - `.env`（先不上传，下一步创建）

### 4.2 创建环境变量文件

1. 进入 `/www/wwwroot/xwchat` 目录
2. 点击 **「新建」** → **「文件」**
3. 命名为 `.env`
4. 编辑内容如下：

```env
PORT=8080
DB_HOST=localhost
DB_PORT=5432
DB_NAME=chatapp
DB_USER=postgres
DB_PASSWORD=你的数据库密码
JWT_SECRET=xwchat_secret_随机字符串_2026
```

> ⚠️ **务必修改**:
> - `DB_PASSWORD`: 改为你的 PostgreSQL 密码
> - `JWT_SECRET`: 改为随机长字符串（可用 `openssl rand -base64 32` 生成）

### 4.3 创建 Node.js 项目

1. 点击左侧 **「Node 项目」** 菜单
2. 点击 **「添加 Node 项目」**

填写配置：

| 配置项 | 值 |
|--------|-----|
| 项目名称 | `xwchat-server` |
| Node 版本 | `20.x` |
| 项目路径 | `/www/wwwroot/xwchat` |
| 启动方式 | `app.js` |
| 项目入口 | `index.js` |
| 安装依赖 | ✅ 勾选 |
| 开机自启 | ✅ 勾选 |
| 端口 | `8080` |

3. 点击 **「提交」**

### 4.4 安装项目依赖

如果自动安装失败，手动操作：

1. 点击项目名称 `xwchat-server`
2. 进入项目详情页
3. 点击 **「模块」** 标签
4. 点击 **「一键安装」** 按钮

等待安装完成，确保以下模块已安装：
- `express`
- `ws`
- `pg`
- `jsonwebtoken`
- `bcryptjs`
- `dotenv`
- `cors`
- `uuid`

### 4.5 启动服务

1. 点击项目详情页的 **「启动选项」** 标签
2. 确认启动命令为 `npm start` 或 `node index.js`
3. 点击 **「启动」** 按钮
4. 查看日志确认启动成功：

```log
[xwchat-server] Server running on port 8080
[xwchat-server] Database connected successfully
```

### 4.6 验证后端服务

在宝塔终端执行：

```bash
curl http://127.0.0.1:8080/api/health
```

应返回 JSON 响应，表示服务正常。

---

## 5. 网站与 SSL 配置

### 5.1 创建网站站点

1. 点击左侧 **「网站」** 菜单
2. 点击 **「添加站点」**

填写配置：

| 配置项 | 值 |
|--------|-----|
| 域名 | `59.110.44.145`（或有域名则填域名） |
| 根目录 | `/www/wwwroot/xwchat_site`（自动创建） |
| PHP 版本 | `纯静态` |
| 数据库 | 无需创建 |

3. 点击 **「提交」**

### 5.2 申请 SSL 证书

#### 方式一：Let's Encrypt（免费，推荐）

1. 在网站列表点击刚创建站点的 **「设置」**
2. 点击左侧 **「SSL」** 标签
3. 选择 **「Let's Encrypt」**
4. 勾选域名，点击 **「申请」**

> ⚠️ **如果申请失败（403 错误）**:
> 1. 检查域名是否已解析到服务器 IP
> 2. 确保 80 端口未被占用
> 3. 暂时关闭 WAF 防火墙
> 4. 改用 DNS 验证方式（需要阿里云 DNS 权限）

#### 方式二：阿里云 SSL 证书

1. 登录 [阿里云 SSL 证书控制台](https://yundun.console.aliyun.com/?p=cas)
2. 申请免费 DV SSL 证书
3. 下载 Nginx 格式证书文件
4. 在宝塔 SSL 设置中选择 **「其他证书」**
5. 上传 `.crt` 和 `.key` 文件
6. 点击 **「保存」**

### 5.3 开启强制 HTTPS

1. 在 SSL 设置页面
2. 开启 **「强制 HTTPS」** 开关
3. 保存配置

---

## 6. Nginx 反向代理配置

### 6.1 添加反向代理

1. 在网站设置弹窗中，点击左侧 **「反向代理」**
2. 点击 **「添加反向代理」**

填写配置：

| 配置项 | 值 |
|--------|-----|
| 代理名称 | `ws_proxy` |
| 目标 URL | `http://127.0.0.1:8080` |
| 发送域名 | `$host` |
| 代理目录 | 留空（代理全部请求） |

3. 点击 **「提交」**

### 6.2 配置 WebSocket 支持

1. 在反向代理列表，点击刚创建的代理右侧的 **「配置文件」**
2. 检查配置是否包含以下内容：

```nginx
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
    
    # WebSocket 长连接超时（重要）
    proxy_read_timeout 86400s;
    proxy_send_timeout 86400s;
}
```

3. 如果缺少，手动添加后点击 **「保存」**

### 6.3 完整 Nginx 配置参考

如需完整控制，可在 **「配置文件」** 中替换为以下内容：

```nginx
server {
    listen 80;
    server_name 59.110.44.145;
    
    # HTTP 重定向到 HTTPS
    return 301 https://$server_name$request_uri;
}

server {
    listen 443 ssl http2;
    server_name 59.110.44.145;
    
    # SSL 证书（宝塔自动管理）
    ssl_certificate /www/server/panel/vhost/cert/59.110.44.145/fullchain.pem;
    ssl_certificate_key /www/server/panel/vhost/cert/59.110.44.145/privkey.pem;
    
    # SSL 优化
    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers HIGH:!aNULL:!MD5;
    ssl_prefer_server_ciphers on;
    ssl_session_cache shared:SSL:10m;
    
    # WebSocket 代理
    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_cache_bypass $http_upgrade;
        proxy_read_timeout 86400s;
        proxy_send_timeout 86400s;
    }
    
    # API 接口
    location /api {
        proxy_pass http://127.0.0.1:8080;
        proxy_http_version 1.1;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }
}
```

### 6.4 验证配置

1. 点击 Nginx 配置页面的 **「保存」**
2. 宝塔会自动检查配置语法
3. 重启 Nginx：点击左侧 **「软件商店」** → Nginx **「设置」** → **「重启」**

---

## 7. 客户端配置

### 7.1 环境变量配置

客户端通过环境变量连接后端，发布包可附带启动脚本。

**Windows 启动脚本 (`start.bat`):**
```batch
@echo off
set CHATAPP_BACKEND_HOST=59.110.44.145
set CHATAPP_BACKEND_PORT=443
set CHATAPP_API_SCHEME=https
set CHATAPP_WS_SCHEME=wss
start ChatApp.exe
```

**配置文件位置**: `src/database/databaseconfig.cpp`
- Release 模式默认主机：`59.110.44.145`
- 支持环境变量覆盖配置

### 7.2 Windows 打包步骤

1. 在 Qt Creator 中以 **Release** 模式编译
2. 打开 Qt 命令行，进入输出目录：
   ```cmd
   cd E:\Project\QtProject\ChatApp\bin\release
   ```
3. 部署 Qt 依赖：
   ```cmd
   windeployqt ChatApp.exe --qmldir E:\Project\QtProject\ChatApp\qml
   ```
4. 复制 PostgreSQL 驱动文件（从 PostgreSQL 安装目录的 `bin` 文件夹）：
   - `libpq.dll`
   - `libintl-9.dll`（版本号可能不同）
   - `libiconv-2.dll`
   - `libcrypto-1_1-x64.dll`
   - `libssl-1_1-x64.dll`
5. 确保 `sqldrivers/qsqlpsql.dll` 存在于打包目录

### 7.3 测试连接

启动客户端后，应能正常：
- 注册/登录账号
- 发送和接收消息
- 添加好友
- 显示在线状态

---

## 8. 常见问题排查

### 8.1 Node 项目启动失败

**症状**: Node 项目状态显示「未运行」或「启动失败」

**排查步骤**:
1. 点击项目 → **「日志」** 查看错误信息
2. 常见错误及解决：

| 错误信息 | 解决方案 |
|----------|----------|
| `Cannot find module 'xxx'` | 点击「模块」→「一键安装」 |
| `Database connection failed` | 检查 `.env` 数据库配置 |
| `Port 8080 is already in use` | 检查端口占用或修改端口 |

3. 检查 `.env` 文件是否存在且配置正确：
   ```bash
   cat /www/wwwroot/xwchat/.env
   ```

### 8.2 数据库连接失败

**症状**: 后端日志显示数据库连接错误

**解决方案**:
1. 检查 PostgreSQL 是否运行：
   - 点击 **「数据库」** → 查看状态
2. 验证数据库密码：
   - 点击数据库右侧 **「权限」** → 重置密码
3. 更新 `.env` 文件，重启 Node 项目

### 8.3 SSL 证书申请失败

**症状**: Let's Encrypt 申请返回 403 错误

**解决方案**:

1. **检查域名解析**:
   ```bash
   ping 59.110.44.145
   ```

2. **关闭 WAF 临时测试**:
   - 点击左侧 **「WAF」** → 暂时关闭

3. **使用 DNS 验证**（推荐）:
   - 在 SSL 申请页面选择「DNS 验证」
   - 选择「阿里云 DNS」
   - 按提示添加 TXT 记录
   - 验证通过后自动颁发证书

4. **手动创建验证目录**:
   ```bash
   mkdir -p /www/wwwroot/xwchat_site/.well-known/acme-challenge
   chmod 755 /www/wwwroot/xwchat_site/.well-known
   ```

### 8.4 WebSocket 连接断开

**症状**: 客户端频繁显示「重新连接中」

**解决方案**:

1. 检查 Nginx 反向代理配置：
   ```nginx
   proxy_set_header Upgrade $http_upgrade;
   proxy_set_header Connection "upgrade";
   proxy_read_timeout 86400s;
   ```

2. 检查宝塔防火墙设置：
   - 点击 **「安全」** → 确保 443 端口放行

3. 查看 WebSocket 日志：
   ```bash
   pm2 logs xwchat-server | grep -i websocket
   ```

### 8.5 502 Bad Gateway

**症状**: 访问网站显示 502 错误

**解决方案**:

1. 检查后端服务状态：
   - 点击 **「Node 项目」** → 查看运行状态
2. 检查反向代理配置：
   - 目标 URL 是否为 `http://127.0.0.1:8080`
3. 重启服务：
   - Node 项目点击「重启」
   - Nginx 点击「重启」

### 8.6 端口冲突

**症状**: 服务启动失败，提示端口被占用

**排查命令**:
```bash
netstat -tlnp | grep 8080
```

**解决方案**:
1. 停止占用端口的进程
2. 或修改 Node 项目端口为其他值（如 8081）
3. 同步修改 Nginx 反向代理目标端口

---

## 📊 宝塔面板日常运维

### 监控服务状态

1. **Node 项目监控**:
   - 点击 **「Node 项目」** → 查看 CPU/内存占用
   - 设置重启计划（可选）

2. **数据库监控**:
   - 点击 **「数据库」** → 查看连接数
   - 定期备份数据

3. **网站监控**:
   - 点击 **「网站」** → 查看访问统计
   - 查看 Nginx 日志

### 定期备份

1. **数据库备份**:
   - 点击 **「数据库」** → 右侧「备份」
   - 设置自动备份计划（建议每天）

2. **文件备份**:
   - 点击 **「文件」** → 选择目录
   - 右键「压缩」→ 下载到本地

3. **整站备份**:
   - 使用宝塔「计划任务」功能
   - 设置每周自动备份到阿里云 OSS

### 日志管理

1. **应用日志**:
   ```bash
   pm2 logs xwchat-server --lines 100
   ```

2. **Nginx 日志**:
   - 网站设置 → 「日志」标签

3. **数据库日志**:
   - PostgreSQL 设置 → 「错误日志」

---

## 🔐 安全加固建议

### 1. 防火墙配置

点击左侧 **「安全」**，确保：

| 端口 | 状态 | 备注 |
|------|------|------|
| 22 | 放行 | SSH |
| 80 | 放行 | HTTP |
| 443 | 放行 | HTTPS |
| 8080 | 拒绝 | 仅内网访问 |
| 5432 | 拒绝 | 仅内网访问 |

### 2. 修改默认配置

- 修改宝塔面板默认端口（8888）
- 修改面板登录用户名和密码
- 修改数据库默认密码
- 修改 `JWT_SECRET` 为强随机字符串

### 3. 启用 WAF

1. 点击 **「软件商店」** → 安装「Nginx 免费防火墙」
2. 启用防火墙
3. 配置防护规则

### 4. 定期更新

- 定期更新宝塔面板
- 定期更新 Node.js 依赖
- 定期更新 SSL 证书（Let's Encrypt 90 天有效期）

---

## 📞 技术支持

收集以下信息以便排查问题：

1. **Node 项目日志**: Node 项目 → 日志
2. **Nginx 错误日志**: 网站设置 → 日志 → 错误日志
3. **数据库状态**: 数据库 → 右侧「设置」
4. **系统资源**: 首页 → 资源占用情况

---

**文档结束**
