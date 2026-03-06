// server.js - 添加心跳响应
const WebSocket = require('ws');
const http = require('http');

const server = http.createServer();
const wss = new WebSocket.Server({ server });

const clients = new Map();

wss.on('connection', (ws) => {
    console.log('🔌 新客户端连接');
    let userId = null;
    let userName = '匿名';
    let lastHeartbeat = Date.now();

    // 心跳检测（每45秒检查一次）
    const heartbeatCheck = setInterval(() => {
        if (Date.now() - lastHeartbeat > 60000) { // 60秒超时
            console.log(`⏰ ${userName} (${userId || '未认证'}) 心跳超时，断开连接`);
            ws.terminate();
            clearInterval(heartbeatCheck);
        }
    }, 45000);

    ws.on('message', (message) => {
        try {
            const data = JSON.parse(message.toString());
            lastHeartbeat = Date.now(); // 更新最后心跳时间

            // 心跳处理 - 立即响应
            if (data.type === 'heartbeat') {
                ws.send(JSON.stringify({ 
                    type: 'heartbeat', 
                    timestamp: Date.now(),
                    serverTime: new Date().toISOString()
                }));
                return;
            }

            // 认证处理
            if (data.type === 'auth') {
                userId = data.userId;
                userName = data.userName;
                clients.set(userId, {
                    ws: ws,
                    userId: userId,
                    userName: userName,
                    connectedAt: Date.now()
                });

                // 认证成功响应
                ws.send(JSON.stringify({
                    type: 'auth',
                    status: 'success',
                    message: `认证成功，欢迎 ${userName}`,
                    userId: userId,
                    timestamp: Date.now()
                }));

                console.log(`✅ 客户端认证成功: ${userName} (${userId})`);
                console.log(`📊 当前在线用户数: ${clients.size}`);
                return;
            }

            // 消息处理 - 广播给所有客户端（除发送者外）
            if (data.type === 'message') {
                const messageData = {
                    type: 'message',
                    senderId: userId,
                    senderName: userName,
                    conversationId: data.conversationId || 'conv_001',
                    content: data.content,
                    timestamp: data.timestamp || Date.now(),
                    status: 'delivered'
                };

                console.log(`📨 收到消息从 ${userName}: "${data.content.substring(0, 30)}..."`);
                console.log(`   会话ID: ${messageData.conversationId}`);

                // 广播给所有连接的客户端（✅ 排除发送者自己）
                let broadcastCount = 0;
                for (const [clientId, client] of clients) {
                    // ✅ 关键：排除发送者自己，避免消息重复显示
                    if (client.ws.readyState === WebSocket.OPEN && clientId !== userId) {
                        client.ws.send(JSON.stringify(messageData));
                        broadcastCount++;
                    }
                }

                console.log(`📤 已广播给 ${broadcastCount} 个其他客户端（排除发送者）`);
                return;
            }

            // 未知消息类型
            console.log(`❓ 未知消息类型: ${data.type}`);
            ws.send(JSON.stringify({
                type: 'error',
                message: `未知消息类型: ${data.type}`
            }));
            
        } catch (e) {
            console.error('❌ 消息处理错误:', e.message);
            ws.send(JSON.stringify({ type: 'error', message: '无效消息格式' }));
        }
    });

    ws.on('close', () => {
        clearInterval(heartbeatCheck);
        if (userId) {
            console.log(`👋 ${userName} (${userId}) 断开连接`);
            clients.delete(userId);
        }
    });

    ws.on('error', (error) => {
        clearInterval(heartbeatCheck);
        console.error('❌ WebSocket 错误:', error.message);
    });
});

const PORT = 8080;
server.listen(PORT, () => {
    console.log(`\n🚀 WebSocket 服务器运行中 (带心跳检测)`);
    console.log(`📍 地址: ws://localhost:${PORT}`);
    console.log(`⏱️  心跳间隔: 30秒 | 超时断开: 60秒\n`);
});