const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

const BASE_URL = process.env.ESCHAT_BASE_URL || 'http://localhost:8080';
const WS_URL = process.env.ESCHAT_WS_URL || 'ws://localhost:8080';
const PASSWORD = 'Passw0rd!123';
const AVATAR_FILE = path.resolve(__dirname, '..', 'image', 'boy.png');

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function requestJson(url, options = {}) {
    const response = await fetch(url, options);
    const text = await response.text();
    let payload = null;

    if (text) {
        try {
            payload = JSON.parse(text);
        } catch (_error) {
            payload = text;
        }
    }

    if (!response.ok) {
        throw new Error(`HTTP ${response.status} ${url} -> ${JSON.stringify(payload)}`);
    }

    return payload;
}

function assert(condition, message) {
    if (!condition) {
        throw new Error(message);
    }
}

class WsProbe {
    constructor(label, token) {
        this.label = label;
        this.token = token;
        this.socket = null;
        this.queue = [];
        this.waiters = [];
    }

    async connect() {
        this.socket = new WebSocket(WS_URL);

        await new Promise((resolve, reject) => {
            const openHandler = () => {
                this.socket.removeEventListener('error', errorHandler);
                resolve();
            };

            const errorHandler = (event) => {
                this.socket.removeEventListener('open', openHandler);
                reject(event.error || new Error(`${this.label} websocket connect failed`));
            };

            this.socket.addEventListener('open', openHandler, { once: true });
            this.socket.addEventListener('error', errorHandler, { once: true });
        });

        this.socket.addEventListener('message', event => {
            const payload = JSON.parse(event.data.toString());
            const waiterIndex = this.waiters.findIndex(waiter => waiter.predicate(payload));

            if (waiterIndex >= 0) {
                const waiter = this.waiters.splice(waiterIndex, 1)[0];
                clearTimeout(waiter.timeoutId);
                waiter.resolve(payload);
                return;
            }

            this.queue.push(payload);
        });

        this.socket.send(JSON.stringify({ type: 'auth', token: this.token }));
        const authMessage = await this.waitFor(message => message.type === 'auth_success', 5000);
        assert(authMessage.userId, `${this.label} websocket auth response missing userId`);
    }

    send(payload) {
        this.socket.send(JSON.stringify(payload));
    }

    waitFor(predicate, timeoutMs = 5000) {
        const queuedIndex = this.queue.findIndex(predicate);
        if (queuedIndex >= 0) {
            return Promise.resolve(this.queue.splice(queuedIndex, 1)[0]);
        }

        return new Promise((resolve, reject) => {
            const timeoutId = setTimeout(() => {
                const waiterIndex = this.waiters.findIndex(waiter => waiter.timeoutId === timeoutId);
                if (waiterIndex >= 0) {
                    this.waiters.splice(waiterIndex, 1);
                }
                reject(new Error(`${this.label} websocket wait timed out after ${timeoutMs}ms`));
            }, timeoutMs);

            this.waiters.push({ predicate, resolve, reject, timeoutId });
        });
    }

    close() {
        if (this.socket && this.socket.readyState < WebSocket.CLOSING) {
            this.socket.close();
        }
    }
}

async function registerAndLogin(username) {
    await requestJson(`${BASE_URL}/api/auth/register`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username, password: PASSWORD })
    });

    const login = await requestJson(`${BASE_URL}/api/auth/login`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username, password: PASSWORD })
    });

    return {
        username,
        userId: login.userId,
        token: login.accessToken
    };
}

async function authorizedJson(pathname, token, options = {}) {
    const headers = {
        Authorization: `Bearer ${token}`,
        ...(options.headers || {})
    };

    return requestJson(`${BASE_URL}${pathname}`, {
        ...options,
        headers
    });
}

async function uploadAvatar(token) {
    const avatarBuffer = fs.readFileSync(AVATAR_FILE);
    const form = new FormData();
    form.append('avatar', new File([avatarBuffer], 'boy.png', { type: 'image/png' }));

    return requestJson(`${BASE_URL}/api/users/profile/avatar`, {
        method: 'POST',
        headers: {
            Authorization: `Bearer ${token}`
        },
        body: form
    });
}

async function main() {
    const suffix = Date.now();
    const alice = await registerAndLogin(`alice_e2e_${suffix}`);
    const bob = await registerAndLogin(`bob_e2e_${suffix}`);

    const aliceWs = new WsProbe('alice', alice.token);
    const bobWs = new WsProbe('bob', bob.token);

    try {
        await aliceWs.connect();
        await bobWs.connect();

        const aliceConversationsBefore = await authorizedJson('/api/conversations', alice.token);
        const bobConversationsBefore = await authorizedJson('/api/conversations', bob.token);
        assert(Array.isArray(aliceConversationsBefore) && aliceConversationsBefore.length === 0, 'alice should start with no conversations');
        assert(Array.isArray(bobConversationsBefore) && bobConversationsBefore.length === 0, 'bob should start with no conversations');

        const friendRequest = await authorizedJson('/api/friends/request', alice.token, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ toUserId: bob.userId })
        });
        assert(friendRequest.requestId, 'friend request should return requestId');

        const bobFriendRequestPush = await bobWs.waitFor(message => message.type === 'friend_request' && message.fromId === alice.userId, 5000);
        assert(bobFriendRequestPush.requestId === friendRequest.requestId, 'bob should receive the pending request over websocket');

        const bobPendingRequests = await authorizedJson('/api/friends/requests', bob.token);
        const pendingRequest = bobPendingRequests.find(item => item.request_id === friendRequest.requestId);
        assert(pendingRequest, 'bob pending request list should contain alice request');

        await authorizedJson(`/api/friends/requests/${friendRequest.requestId}`, bob.token, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ status: 'accepted' })
        });

        const aliceAcceptedPush = await aliceWs.waitFor(message => message.type === 'friend_response' && message.toId === bob.userId, 5000);
        assert(aliceAcceptedPush.conversationId, 'alice should receive conversationId after acceptance');
        const conversationId = aliceAcceptedPush.conversationId;

        const aliceFriends = await authorizedJson('/api/friends', alice.token);
        const bobFriends = await authorizedJson('/api/friends', bob.token);
        assert(aliceFriends.some(item => item.user_id === bob.userId), 'alice friends list should contain bob');
        assert(bobFriends.some(item => item.user_id === alice.userId), 'bob friends list should contain alice');

        const aliceConversationsAfter = await authorizedJson('/api/conversations', alice.token);
        const bobConversationsAfter = await authorizedJson('/api/conversations', bob.token);
        const aliceConversation = aliceConversationsAfter.find(item => item.conversation_id === conversationId);
        const bobConversation = bobConversationsAfter.find(item => item.conversation_id === conversationId);
        assert(aliceConversation, 'alice conversations should include the accepted single chat');
        assert(bobConversation, 'bob conversations should include the accepted single chat');
        assert(aliceConversation.name === bob.username, 'alice conversation title should be bob username');
        assert(bobConversation.name === alice.username, 'bob conversation title should be alice username');

        const chatText = `hello-from-contact-path-${suffix}`;
        bobWs.send({
            type: 'message',
            conversationId: alice.userId,
            content: chatText,
            timestamp: Date.now()
        });

        const bobMessageAck = await bobWs.waitFor(
            message => message.type === 'message_ack' && message.success === true && message.clientMessageId === null,
            5000
        );
        assert(bobMessageAck.messageId, 'sender should receive message_ack with server messageId');
        assert(bobMessageAck.conversationId === conversationId, 'message_ack should resolve to the single chat conversationId');

        const aliceIncomingMessage = await aliceWs.waitFor(
            message => message.type === 'message' && message.senderId === bob.userId && message.content === chatText,
            5000
        );
        assert(aliceIncomingMessage.conversationId === conversationId, 'contact-entry message should resolve to the final single conversationId');

        await sleep(300);
        const history = await authorizedJson(`/api/messages/${conversationId}`, alice.token);
        assert(history.some(item => item.content === chatText), 'message history should contain the contact-path message');

        const refreshAfterMessage = await authorizedJson('/api/conversations', alice.token);
        const refreshedConversation = refreshAfterMessage.find(item => item.conversation_id === conversationId);
        assert(refreshedConversation && refreshedConversation.last_message === chatText, 'conversation refresh should reflect the latest message');

        bobWs.send({
            type: 'recall_message',
            conversationId,
            messageId: bobMessageAck.messageId,
            timestamp: Date.now()
        });

        const bobRecallAck = await bobWs.waitFor(
            message => message.type === 'message_recalled' && message.success === true && message.messageId === bobMessageAck.messageId,
            5000
        );
        assert(bobRecallAck.conversationId === conversationId, 'sender should receive message_recalled for the same conversation');

        const aliceRecallPush = await aliceWs.waitFor(
            message => message.type === 'message_recalled' && message.success === true && message.messageId === bobMessageAck.messageId,
            5000
        );
        assert(aliceRecallPush.conversationId === conversationId, 'receiver should receive message_recalled for the same conversation');

        await sleep(300);
        const historyAfterRecall = await authorizedJson(`/api/messages/${conversationId}`, alice.token);
        const recalledMessage = historyAfterRecall.find(item => item.message_id === bobMessageAck.messageId);
        assert(recalledMessage && recalledMessage.recalled === true, 'message history should mark recalled messages');

        const sinceBeforeProfileUpdate = new Date().toISOString();
        const avatarUpload = await uploadAvatar(alice.token);
        assert(avatarUpload.avatarPath, 'avatar upload should return avatarPath');
        const avatarFetch = await fetch(`${BASE_URL}${avatarUpload.avatarPath}`);
        assert(avatarFetch.ok, 'uploaded avatar should be reachable from static files');

        const renamedAlice = `alice_e2e_renamed_${suffix}`;
        const profileUpdate = await authorizedJson('/api/users/profile', alice.token, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                username: renamedAlice,
                avatar: avatarUpload.avatarPath,
                region: 'E2E-Lab',
                bio: 'updated by e2e'
            })
        });
        assert(profileUpdate.profile && profileUpdate.profile.username === renamedAlice, 'profile update response should contain the new username');

        const bobProfilePush = await bobWs.waitFor(
            message => message.type === 'profile_updated' && message.userId === alice.userId,
            5000
        );
        assert(bobProfilePush.username === renamedAlice, 'friend websocket update should contain the renamed username');
        assert(bobProfilePush.avatar === avatarUpload.avatarPath, 'friend websocket update should contain the updated avatar path');
        assert(bobProfilePush.status === 'online', 'friend websocket update should keep the current online status');
        assert(bobProfilePush.signature === 'updated by e2e', 'friend websocket update should contain the latest signature');

        const bobFriendDetail = await authorizedJson(`/api/friends/${alice.userId}`, bob.token);
        assert(bobFriendDetail.username === renamedAlice, 'friend detail API should expose the renamed username');
        assert(bobFriendDetail.avatar === avatarUpload.avatarPath, 'friend detail API should expose the updated avatar');
        assert(bobFriendDetail.signature === 'updated by e2e', 'friend detail API should expose the latest signature');

        const bobFriendUpdates = await authorizedJson(`/api/friends/updates/since?since=${encodeURIComponent(sinceBeforeProfileUpdate)}`, bob.token);
        const aliceUpdate = bobFriendUpdates.updates.find(item => item.user_id === alice.userId);
        assert(aliceUpdate, 'friends/updates/since should return the updated friend profile');
        assert(aliceUpdate.username === renamedAlice, 'friends/updates/since should return the new username');
        assert(aliceUpdate.signature === 'updated by e2e', 'friends/updates/since should return the updated signature');

        const bobConversationsRefreshed = await authorizedJson('/api/conversations', bob.token);
        const bobConversationRefreshed = bobConversationsRefreshed.find(item => item.conversation_id === conversationId);
        assert(bobConversationRefreshed.name === renamedAlice, 'conversation refresh should use the renamed friend username');
        assert(bobConversationRefreshed.avatar === avatarUpload.avatarPath, 'conversation refresh should use the updated friend avatar');

        console.log(JSON.stringify({
            ok: true,
            users: {
                alice: { userId: alice.userId, oldUsername: alice.username, newUsername: renamedAlice },
                bob: { userId: bob.userId, username: bob.username }
            },
            conversationId,
            checks: [
                '好友申请 websocket 推送',
                '好友接受后会话创建',
                '消息 ack 返回 serverMessageId',
                '联系人入口 userId -> 单聊 conversationId 映射',
                '消息历史与会话列表刷新',
                '消息撤回 websocket 广播与历史状态',
                '头像上传静态访问',
                '昵称和头像修改 websocket 推送',
                '好友详情与增量更新接口',
                '会话列表刷新后的标题和头像同步'
            ]
        }, null, 2));
    } finally {
        aliceWs.close();
        bobWs.close();
    }
}

main().catch(error => {
    console.error(error.stack || error.message || error);
    process.exit(1);
});