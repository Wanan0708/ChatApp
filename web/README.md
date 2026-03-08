# ESChat 前端网站

ESChat 即时通讯应用的现代化前端网站，提供用户登录注册、客户端下载等功能。

## 📁 目录结构

```
web/
├── index.html          # 首页（登录/注册）
├── download.html       # 下载页面
├── about.html          # 关于页面
├── css/
│   ├── style.css       # 通用样式
│   ├── download.css    # 下载页面样式
│   └── about.css       # 关于页面样式
├── js/
│   └── main.js         # JavaScript 逻辑
├── images/             # 图片资源
└── downloads/          # 客户端安装包目录
```

## 🚀 功能特性

- ✨ **现代化设计**：采用渐变色、卡片式布局，视觉效果时尚现代
- 📱 **响应式布局**：完美适配桌面端和移动端
- 🔐 **用户认证**：集成登录注册功能，支持 JWT Token 认证
- 📥 **下载管理**：多平台客户端下载，支持 Windows、macOS、Linux
- 🎨 **自定义主题**：CSS 变量配置，轻松修改主题色

## 🛠️ 本地开发

### 前置要求

确保后端服务器已启动并运行在 `http://localhost:8080`

### 启动方式

1. 确保项目目录结构正确：
   ```
   ChatApp/
   ├── server/
   │   └── index.js
   └── web/
       └── index.html
   ```

2. 启动后端服务器（会自动提供静态文件服务）：
   ```bash
   cd server
   npm start
   ```

3. 访问网站：
   - 首页：http://localhost:8080/
   - 下载页：http://localhost:8080/download.html
   - 关于页：http://localhost:8080/about.html

## 📦 服务器部署

### 宝塔面板部署

1. **上传文件**
   - 将整个 `web/` 目录上传到服务器项目目录

2. **配置 Nginx**
   - 确保 Nginx 反向代理配置正确
   - `/api` 请求代理到 Node.js 服务
   - 静态文件由 Node.js 直接服务

3. **放置客户端安装包**
   - 将编译好的客户端安装包放入 `web/downloads/` 目录
   - 推荐文件名：
     - `ESChat-Setup-Windows.exe`
     - `ESChat-Setup-macOS.dmg`
     - `ESChat-Setup-Linux.AppImage`

### API 配置

前端会自动根据环境配置 API 地址：

```javascript
// js/main.js
const API_BASE_URL = window.location.hostname === 'localhost' 
    ? 'http://localhost:8080/api' 
    : '/api';  // 生产环境使用相对路径
```

生产环境需确保 Nginx 配置将 `/api` 请求代理到 Node.js 服务。

## 🎨 自定义配置

### 修改主题色

编辑 `css/style.css` 中的 CSS 变量：

```css
:root {
    --primary-color: #6366f1;      /* 主色调 */
    --primary-hover: #4f46e5;      /* 悬停色 */
    --primary-light: #e0e7ff;      /* 浅色背景 */
    --secondary-color: #8b5cf6;    /* 辅助色 */
}
```

### 修改网站信息

编辑各 HTML 文件中的相关内容：

- **网站标题**：修改 `<title>` 标签
- **联系方式**：编辑 `about.html` 中的联系信息
- **统计数据**：编辑 `index.html` 中的统计数据

### 修改 Logo

替换 `index.html`、`download.html`、`about.html` 中的 Logo 部分：

```html
<div class="logo">
    <span class="logo-icon">💬</span>
    <span class="logo-text">ESChat</span>
</div>
```

## 📊 主要页面说明

### 首页 (index.html)

- Hero 区域：展示产品标语和下载引导
- 功能特性：6 个核心功能卡片展示
- 登录/注册模态框：用户认证入口
- 聊天预览动画：动态展示聊天效果

### 下载页 (download.html)

- 多平台下载卡片：Windows、macOS、Linux
- 安装指南：3 步安装流程说明
- 系统要求：各平台详细配置要求
- 其他下载方式：MSI、PKG、DEB、RPM 等

### 关于页 (about.html)

- 产品介绍：产品特性和优势
- 技术架构：客户端、后端、数据库、安全技术栈
- 联系方式：电子邮件、客服、地址
- 版本信息：当前版本和更新日志

## 🔌 API 接口

前端调用以下后端 API 接口：

| 接口 | 方法 | 说明 |
|------|------|------|
| `/api/auth/login` | POST | 用户登录 |
| `/api/auth/register` | POST | 用户注册 |
| `/api/downloads` | GET | 获取下载文件列表 |
| `/api/download/:filename` | GET | 下载指定文件 |

## 📝 注意事项

1. **安全配置**
   - 生产环境务必启用 HTTPS
   - 确保 JWT_SECRET 环境变量已正确配置
   - 定期更新依赖包

2. **性能优化**
   - 建议启用 Gzip 压缩
   - 图片资源建议使用 WebP 格式
   - 可考虑使用 CDN 加速静态资源

3. **浏览器兼容性**
   - 推荐使用现代浏览器（Chrome、Firefox、Safari、Edge）
   - 最低支持版本：Chrome 60+、Firefox 55+、Safari 11+

## 📄 许可证

© 2026 ESChat. All rights reserved.
