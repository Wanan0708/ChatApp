# ESChat 前端网站部署说明

## 📋 目录结构

```
ChatApp/
├── server/                 # Node.js 后端服务
│   ├── index.js           # 主服务器文件（已配置静态文件服务）
│   └── ...
├── web/                    # 前端网站
│   ├── index.html         # 首页
│   ├── download.html      # 下载页
│   ├── about.html         # 关于页
│   ├── css/               # 样式文件
│   ├── js/                # JavaScript 文件
│   └── downloads/         # 客户端下载目录（放置 ZIP 压缩包）
└── docs/                   # 文档
```

---

## 🌐 域名访问原理

### 完整流程

```
1. 用户访问 https://your-domain.com/
   ↓
2. DNS 解析域名到服务器 IP
   ↓
3. Nginx（宝塔面板）接收请求
   ↓
4. 根据反向代理配置：
   - 静态资源 → Node.js 的 express.static() 处理
   - /api/* 请求 → 代理到 http://127.0.0.1:8080
   ↓
5. Node.js 返回对应的 HTML/CSS/JS 文件
   ↓
6. 浏览器渲染页面
```

### 宝塔面板配置步骤

#### 1. 创建网站

1. 登录宝塔面板
2. 进入「网站」→「添加站点」
3. 填写域名（如：`chat.yourdomain.com`）
4. 根目录选择：`/www/wwwroot/your-site`

#### 2. 上传项目文件

1. 通过 FTP 或宝塔文件管理器
2. 将整个项目上传到网站根目录
3. 确保目录结构：
   ```
   /www/wwwroot/your-site/
   ├── server/
   ├── web/
   └── ...
   ```

#### 3. 配置 Node.js 项目

1. 宝塔面板 → 「Node 项目」→「添加 Node 项目」
2. 项目目录：`/www/wwwroot/your-site/server`
3. 启动文件：`index.js`
4. 端口：`8080`

#### 4. 配置反向代理

1. 网站设置 → 「反向代理」→「添加反向代理」
2. 配置：
   - 代理名称：`chat-api`
   - 目标 URL：`http://127.0.0.1:8080`
   - 发送域名：`$host`
   - 代理目录：`/api`

3. 提交后，编辑配置文件，确保包含：
   ```nginx
   location /api {
       proxy_pass http://127.0.0.1:8080;
       proxy_http_version 1.1;
       proxy_set_header Upgrade $http_upgrade;
       proxy_set_header Connection "upgrade";
       proxy_set_header Host $host;
       proxy_set_header X-Real-IP $remote_addr;
   }

   location / {
       proxy_pass http://127.0.0.1:8080;
       proxy_http_version 1.1;
       proxy_set_header Upgrade $http_upgrade;
       proxy_set_header Connection "upgrade";
       proxy_set_header Host $host;
   }
   ```

#### 5. 配置 SSL（HTTPS）

1. 网站设置 → 「SSL」→ 「Let's Encrypt」
2. 申请免费 SSL 证书
3. 强制 HTTPS

---

## 📦 客户端打包部署

### 步骤 1：Qt 打包

#### Windows
```bash
# 在项目目录下执行
windeployqt release/ChatApp.exe
```

#### macOS
```bash
macdeployqt ChatApp.app
```

#### Linux
```bash
linuxdeployqt ChatApp -appimage
```

### 步骤 2：整理打包文件

将 Qt 打包生成的所有文件整理到一个文件夹：

```
ChatApp-Windows/
├── ChatApp.exe
├── *.dll (Qt 依赖)
├── image/
├── qml/
└── ...
```

### 步骤 3：压缩成 ZIP

**Windows：**
- 右键文件夹 → 发送到 → 压缩 (zipped) 文件夹
- 重命名为 `ESChat-Windows.zip`

**macOS/Linux：**
```bash
zip -r ESChat-Windows.zip ChatApp-Windows/
```

### 步骤 4：上传到服务器

将 ZIP 文件上传到：
```
web/downloads/ESChat-Windows.zip
web/downloads/ESChat-macOS.zip
web/downloads/ESChat-Linux.zip
```

---

## 🚀 本地测试

### 方法 1：直接启动 Node.js 服务器

```bash
cd server
npm start
```

访问：http://localhost:8080

### 方法 2：使用 Qt Creator 运行

1. 运行 Qt 项目（ChatApp）
2. 单独启动 Node.js 服务器
3. 确保 `.env` 配置正确

---

## ✅ 验证部署

### 1. 检查网站访问

- [ ] 首页能正常访问
- [ ] 下载页面能正常访问
- [ ] 关于页面能正常访问
- [ ] 登录/注册模态框能正常弹出

### 2. 检查 API 连接

打开浏览器控制台，测试：
```javascript
fetch('/api/downloads')
  .then(r => r.json())
  .then(d => console.log(d))
```

应该返回：
```json
{
  "files": [
    {
      "name": "ESChat-Windows.zip",
      "size": 123456789,
      "sizeFormatted": "117.73 MB",
      "downloadUrl": "/downloads/ESChat-Windows.zip"
    }
  ]
}
```

### 3. 检查下载功能

- [ ] 点击下载按钮能开始下载
- [ ] 下载的文件完整
- [ ] 解压后能正常运行

---

## 🔧 常见问题

### Q1: 访问域名显示 404

**解决：**
- 检查 Nginx 配置是否正确
- 确认 Node.js 服务已启动
- 检查文件路径是否正确

### Q2: API 请求失败

**解决：**
- 检查反向代理配置
- 确认 Node.js 端口（默认 8080）
- 查看防火墙设置

### Q3: 下载文件 404

**解决：**
- 确认文件已上传到 `web/downloads/`
- 检查文件名是否匹配
- 查看文件权限设置

### Q4: WebSocket 连接失败

**解决：**
- 确保 Nginx 配置了 WebSocket 支持
- 检查 `proxy_set_header Upgrade $http_upgrade`
- 确认 SSL 证书有效

---

## 📞 技术支持

如遇到问题，请检查：

1. **Node.js 日志**：宝塔面板 → Node 项目 → 日志
2. **Nginx 错误日志**：网站设置 → 日志 → 错误日志
3. **浏览器控制台**：F12 → Console

---

**文档版本：** 1.0.0  
**更新日期：** 2026 年 3 月 6 日
