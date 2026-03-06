# XWChat - 现代化即时通讯应用

<div align="center">

💬 即时通讯，连接你我

[![Qt](https://img.shields.io/badge/Qt-5.12+-blue.svg)](https://www.qt.io/)
[![Node.js](https://img.shields.io/badge/Node.js-16+-green.svg)](https://nodejs.org/)
[![PostgreSQL](https://img.shields.io/badge/PostgreSQL-10+-blue.svg)](https://www.postgresql.org/)
[![License](https://img.shields.io/badge/license-MIT-yellow.svg)](LICENSE)

</div>

---

## 📖 项目简介

XWChat 是一款现代化的即时通讯应用，采用 Qt/QML 客户端 + Node.js 后端架构，支持实时消息、好友管理、群组聊天等功能。界面时尚现代，支持跨平台部署。

### ✨ 核心功能

- ⚡ **实时消息** - 基于 WebSocket 技术，消息即时送达
- 👥 **好友管理** - 添加好友、管理联系人列表
- 💬 **群组聊天** - 支持多人同时在线聊天
- 🔒 **安全认证** - JWT Token 认证，保障通讯安全
- 📱 **跨平台** - 支持 Windows、macOS、Linux
- 🎨 **现代化 UI** - 时尚美观的界面设计

---

## 🏗️ 技术架构

```
┌─────────────────┐         ┌─────────────────┐         ┌─────────────────┐
│   Qt/QML 客户端  │ ◄────► │  Node.js 后端   │ ◄────► │   PostgreSQL    │
│   (C++/QML)     │ WebSocket│  (Express.js)  │  SQL   │    数据库       │
└─────────────────┘         └─────────────────┘         └─────────────────┘
       ▲
       │
┌─────────────────┐
│   前端网站      │
│  (HTML/CSS/JS)  │
└─────────────────┘
```

### 技术栈

| 组件 | 技术 | 说明 |
|------|------|------|
| **客户端** | Qt 5.12+ / QML 2.12 | 跨平台桌面应用 |
| **后端** | Node.js 16+ / Express 5 | RESTful API + WebSocket |
| **数据库** | PostgreSQL 10+ | 关系型数据库 |
| **前端网站** | HTML5 / CSS3 / JavaScript | 现代化响应式网站 |
| **认证** | JWT / bcrypt | Token 认证 + 密码加密 |

---

## 📁 项目结构

```
ChatApp/
├── src/                    # 客户端源码
│   ├── core/              # 核心功能
│   ├── models/            # 数据模型
│   ├── dao/               # 数据访问层
│   ├── services/          # 业务服务
│   ├── network/           # 网络通信
│   └── utils/             # 工具类
├── qml/                    # QML 界面文件
│   ├── pages/             # 页面组件
│   └── components/        # 通用组件
├── server/                 # Node.js 后端
│   ├── index.js           # 主服务器文件
│   └── package.json       # 依赖配置
├── web/                    # 前端网站
│   ├── index.html         # 首页
│   ├── download.html      # 下载页
│   ├── about.html         # 关于页
│   ├── css/               # 样式文件
│   ├── js/                # JavaScript 文件
│   └── downloads/         # 客户端下载目录
├── docs/                   # 文档
└── database/               # 数据库脚本
```

---

## 🚀 快速开始

### 环境要求

#### 后端
- Node.js 16+（推荐 v18+）
- PostgreSQL 10+（推荐 v14+）
- npm 8+

#### 客户端
- Qt 5.12+（推荐 Qt 5.14+）
- 编译器：MSVC 2019+ 或 MinGW
- 操作系统：Windows 10+ / macOS 10.15+ / Linux

### 1. 数据库配置

```bash
# 创建数据库
createdb -U postgres xwchat

# 导入表结构
psql -U postgres -d xwchat -f database/init_db.sql
```

### 2. 启动后端服务

```bash
# 进入服务器目录
cd server

# 安装依赖
npm install

# 配置环境变量（复制 .env.example 为 .env 并修改）
cp .env.example .env

# 启动服务
npm start
```

后端服务将在 `http://localhost:8080` 启动。

### 3. 启动客户端

```bash
# 使用 Qt Creator 打开 ChatApp.pro
# 点击运行按钮

# 或使用命令行
qmake ChatApp.pro
make
./ChatApp
```

### 4. 访问前端网站

启动后端服务后，访问：
- **首页**: `http://localhost:8080/`
- **下载页**: `http://localhost:8080/download.html`
- **关于页**: `http://localhost:8080/about.html`

---

## 📦 部署指南

### 本地开发

1. 确保 PostgreSQL 服务已启动
2. 启动 Node.js 后端：`cd server && npm start`
3. 运行 Qt 客户端

### 服务器部署（宝塔面板）

详细部署步骤请参考：[docs/aliyun_bt_panel_guide.md](docs/aliyun_bt_panel_guide.md)

**简要步骤：**

1. **安装宝塔面板**
   ```bash
   # Ubuntu
   wget -O install.sh https://download.bt.cn/install/install-ubuntu_6.0.sh && bash install.sh
   ```

2. **安装环境软件**
   - Node.js 16+
   - PostgreSQL 14+
   - Nginx

3. **配置数据库**
   - 创建数据库和用户
   - 导入表结构

4. **部署后端**
   - 上传 `server/` 目录
   - 执行 `npm install`
   - 配置 `.env` 文件
   - 启动 Node.js 项目

5. **配置前端网站**
   - 上传 `web/` 目录到网站根目录
   - 配置 Nginx 反向代理

6. **配置 SSL 证书**
   - 申请免费 Let's Encrypt 证书
   - 启用 HTTPS

---

## 🔧 配置说明

### 后端配置 (.env)

```bash
# 服务器端口
PORT=8080

# 数据库配置
DB_HOST=localhost
DB_PORT=5432
DB_NAME=xwchat
DB_USER=postgres
DB_PASSWORD=your_password

# JWT 密钥
JWT_SECRET=your_jwt_secret_key

# WebSocket 服务器地址（客户端使用）
WEBSOCKET_URL=ws://localhost:8080
```

### 客户端配置

客户端会自动连接后端服务，无需额外配置。如需修改服务器地址，请修改网络相关配置。

---

## 📖 功能说明

### 用户功能
- ✅ 用户注册/登录
- ✅ 个人资料编辑
- ✅ 密码重置

### 好友功能
- ✅ 搜索用户
- ✅ 发送好友申请
- ✅ 处理好友申请
- ✅ 好友列表管理

### 聊天功能
- ✅ 实时消息发送/接收
- ✅ 消息历史记录
- ✅ 已读/未读状态
- ✅ 会话列表管理

---

## 🛠️ 常见问题

### 1. 后端启动失败

**问题**: `Error: Cannot find module 'xxx'`

**解决**: 
```bash
cd server
npm install
```

### 2. 数据库连接失败

**问题**: `database connection failed`

**解决**:
- 检查 PostgreSQL 服务是否启动
- 检查 `.env` 中的数据库配置是否正确
- 确认数据库已创建

### 3. 客户端无法连接后端

**问题**: 登录后无法加载消息

**解决**:
- 检查后端服务是否运行
- 检查防火墙设置
- 确认 WebSocket 地址配置正确

### 4. 前端网站无法访问

**问题**: 访问域名显示 404

**解决**:
- 检查 Nginx 反向代理配置
- 确认 Node.js 服务已启动
- 检查文件路径权限

---

## 📝 开发指南

### 添加新功能

1. **后端 API**
   - 在 `server/index.js` 添加路由
   - 实现业务逻辑

2. **客户端**
   - 在 `src/` 添加 C++ 代码
   - 在 `qml/` 添加 QML 界面

3. **数据库**
   - 在 `database/` 添加迁移脚本

### 代码规范

- 使用 ESLint/Prettier 格式化代码
- 遵循 Qt 编码规范
- 提交前运行测试

---

## 📄 许可证

本项目采用 MIT 许可证。详见 [LICENSE](LICENSE) 文件。

---

## 🤝 贡献

欢迎贡献代码、报告问题或提出建议！

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

---

## 📞 联系方式

- **项目主页**: [GitHub](https://github.com/yourusername/XWChat)
- **问题反馈**: [Issues](https://github.com/yourusername/XWChat/issues)

---

<div align="center">

**© 2026 XWChat. All rights reserved.**

</div>
