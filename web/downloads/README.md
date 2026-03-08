# 客户端下载目录

## 📦 文件放置说明

由于你使用的是 Qt 打包工具直接打包的文件夹（而非安装程序），需要将打包好的文件夹**压缩成 ZIP 文件**后放置在此目录。

### 推荐的文件命名

| 平台 | 文件名 | 说明 |
|------|--------|------|
| Windows | `ESChat-Windows.zip` | Windows 客户端压缩包 |
| macOS | `ESChat-macOS.zip` | macOS 客户端压缩包 |
| Linux | `ESChat-Linux.zip` | Linux 客户端压缩包 |

---

## 📝 打包步骤

### Windows 平台

1. **Qt Creator 打包**
   - 使用 `windeployqt` 工具部署依赖
   - 或使用 Qt Creator 的 "Deploy" 功能

2. **整理文件**
   ```
   ChatApp-Windows/
   ├── ChatApp.exe
   ├── Qt5Core.dll
   ├── Qt5Gui.dll
   ├── Qt5Qml.dll
   ├── ... (其他依赖)
   └── image/
   └── qml/
   ```

3. **压缩成 ZIP**
   - 右键文件夹 → 发送到 → 压缩 (zipped) 文件夹
   - 或使用 7-Zip、WinRAR 等工具

4. **上传到服务器**
   - 将 `ESChat-Windows.zip` 上传到 `web/downloads/` 目录

### macOS 平台

1. **Qt Creator 打包**
   - 使用 `macdeployqt` 工具
   ```bash
   macdeployqt ChatApp.app
   ```

2. **压缩成 ZIP**
   ```bash
   zip -r ESChat-macOS.zip ChatApp.app
   ```

3. **上传到服务器**

### Linux 平台

1. **Qt Creator 打包**
   - 使用 `linuxdeployqt` 工具
   - 或手动复制依赖库

2. **压缩成 ZIP**
   ```bash
   zip -r ESChat-Linux.zip ChatApp/
   ```

3. **上传到服务器**

---

## 📥 用户下载后的使用方式

### Windows 用户
1. 下载 `ESChat-Windows.zip`
2. 解压到任意目录
3. 双击 `ChatApp.exe` 运行

### macOS 用户
1. 下载 `ESChat-macOS.zip`
2. 解压得到 `ChatApp.app`
3. 双击运行（可能需要允许来自"身份不明的开发者"）

### Linux 用户
1. 下载 `ESChat-Linux.zip`
2. 解压到任意目录
3. 赋予执行权限并运行
   ```bash
   chmod +x ChatApp
   ./ChatApp
   ```

---

## 🔧 服务器端配置

确保 Node.js 服务器已正确配置静态文件服务：

```javascript
// server/index.js 中已配置
const downloadsPath = path.join(__dirname, '..', 'web', 'downloads');
app.use('/downloads', express.static(downloadsPath));
```

---

## 📊 当前文件列表

在此目录下放置打包好的 ZIP 文件后，用户可以通过以下方式查看：

- 访问 `/api/downloads` 查看可用文件列表
- 访问 `/downloads/文件名.zip` 直接下载

---

## ⚠️ 注意事项

1. **文件大小**：建议每个平台的压缩包控制在 200MB 以内
2. **定期更新**：每次发布新版本时更新压缩包
3. **版本说明**：建议在压缩包中包含 `README.txt` 说明使用方法
4. **安全检查**：确保压缩包中不包含敏感信息（如数据库密码、API 密钥等）
