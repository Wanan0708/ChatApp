# ESChat 网站部署说明

本文件只描述 web 目录相关的部署约定；完整的服务启动和客户端部署说明统一见 docs/运行部署指南.md。

## 目录约定

```text
web/
├── index.html
├── about.html
├── download.html
├── css/
├── js/
├── images/
└── downloads/
```

## 部署原则

- 网站静态资源由 server/index.js 提供
- 外部入口通常由 Nginx 反向代理到 127.0.0.1:8080
- API 与页面共用同一个域名和服务入口

## 部署步骤

### 1. 上传项目

将 server 和 web 目录部署到同一台服务器，并保证 server/index.js 能读取到 web 目录静态文件。

### 2. 启动 Node 服务

```bash
cd server
npm install
npm start
```

### 3. 配置反向代理

Nginx 最少应把所有站点流量转发到 127.0.0.1:8080，包括 WebSocket 升级头：

```nginx
location / {
    proxy_pass http://127.0.0.1:8080;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
}
```

## 下载包发布

客户端安装包统一放在 web/downloads 目录下，例如：

- web/downloads/ESChat-Windows.zip
- web/downloads/ESChat-macOS.zip
- web/downloads/ESChat-Linux.zip

## 验证项

- 首页可访问
- download.html 可访问
- about.html 可访问
- /api/downloads 能返回下载列表
- 下载文件能正常返回
