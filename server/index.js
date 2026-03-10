// server/index.js
require('dotenv').config();
const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const cors = require('cors');
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const crypto = require('crypto');
const multer = require('multer');
const { Pool } = require('pg');
const { v4: uuidv4 } = require('uuid');
const path = require('path');
const fs = require('fs');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ noServer: true });

const MESSAGE_TYPE_TO_DB = {
    0: 'text',
    1: 'image',
    2: 'file',
    3: 'system'
};

const DB_MESSAGE_TYPE_TO_CLIENT = {
    text: 0,
    image: 1,
    file: 2,
    system: 3
};

const buildProfileUpdateNotification = (row) => ({
    type: 'profile_updated',
    userId: row.user_id,
    username: row.username || '',
    avatar: row.avatar || '',
    gender: row.gender || '',
    region: row.region || '',
    signature: row.bio || row.signature || '',
    age: row.age || 25,
    version: row.profile_version || 1,
    updatedAt: row.profile_updated_at || row.updated_at || new Date().toISOString()
});

const buildPresenceUpdateNotification = (row) => ({
    type: 'presence_update',
    userId: row.user_id,
    status: row.status || 'offline',
    updatedAt: row.updated_at || row.last_seen || new Date().toISOString()
});

// 数据库连接池
const pool = new Pool({
    host: process.env.DB_HOST,
    port: process.env.DB_PORT,
    database: process.env.DB_NAME,
    user: process.env.DB_USER,
    password: process.env.DB_PASSWORD,
});

const withTransaction = async (work) => {
    const client = await pool.connect();

    try {
        await client.query('BEGIN');
        const result = await work(client);
        await client.query('COMMIT');
        return result;
    } catch (error) {
        try {
            await client.query('ROLLBACK');
        } catch (rollbackError) {
            console.error('[DB] Transaction rollback failed:', rollbackError);
        }
        throw error;
    } finally {
        client.release();
    }
};

app.use(cors());
app.use(express.json());

// 静态文件服务 - 前端网站
const webPath = path.join(__dirname, '..', 'web');
const avatarUploadDir = path.join(webPath, 'uploads', 'avatars');
const imageUploadDir = path.join(webPath, 'uploads', 'images');
const fileUploadDir = path.join(webPath, 'uploads', 'files');
fs.mkdirSync(avatarUploadDir, { recursive: true });
fs.mkdirSync(imageUploadDir, { recursive: true });
fs.mkdirSync(fileUploadDir, { recursive: true });

if (fs.existsSync(webPath)) {
    app.use(express.static(webPath));
    console.log('[Static] Serving frontend from:', webPath);
} else {
    console.warn('[Static] Web directory not found:', webPath);
}

// 下载目录静态文件服务
const downloadsPath = path.join(__dirname, '..', 'web', 'downloads');
if (fs.existsSync(downloadsPath)) {
    app.use('/downloads', express.static(downloadsPath));
    console.log('[Static] Serving downloads from:', downloadsPath);
}

// 请求日志中间件
app.use((req, res, next) => {
    console.log(`[HTTP] ${req.method} ${req.path}`);
    next();
});

const REQUIRED_USER_COLUMNS = [
    'gender',
    'region',
    'phone',
    'email',
    'contact',
    'bio',
    'age',
    'status',
    'last_seen',
    'profile_version',
    'profile_updated_at'
];

const REQUIRED_MESSAGE_COLUMNS = [
    'status',
    'file_id',
    'file_name',
    'file_size',
    'file_url',
    'thumbnail_url',
    'recalled'
];

const normalizeMessageType = (value) => {
    if (typeof value === 'number' && Number.isFinite(value)) {
        return MESSAGE_TYPE_TO_DB[value] || 'text';
    }

    const normalized = String(value || '').trim().toLowerCase();
    if (normalized in DB_MESSAGE_TYPE_TO_CLIENT) {
        return normalized;
    }
    return 'text';
};

const messageTypeToClientValue = (value) => {
    const normalized = normalizeMessageType(value);
    return DB_MESSAGE_TYPE_TO_CLIENT[normalized] ?? 0;
};

const buildManagedUploadUrl = (req, relativePath) => `${req.protocol}://${req.get('host')}${relativePath}`;

const createUploadStorage = (directory, allowedExtensions, filenamePrefix) => multer.diskStorage({
    destination: (_req, _file, cb) => {
        cb(null, directory);
    },
    filename: (req, file, cb) => {
        const extension = path.extname(file.originalname || '').toLowerCase();
        const safeExtension = allowedExtensions.includes(extension) ? extension : '';
        const userPrefix = req.user && req.user.userId ? req.user.userId : 'anonymous';
        cb(null, `${filenamePrefix}-${userPrefix}-${Date.now()}-${crypto.randomBytes(6).toString('hex')}${safeExtension}`);
    }
});

const notifyFriendsProfileUpdated = async (userId, userRow = null) => {
    let row = userRow;
    if (!row) {
        const result = await pool.query(
            `SELECT user_id, username, avatar, gender, region,
                    bio, age, status, profile_version, profile_updated_at, updated_at
             FROM users
             WHERE user_id = $1`,
            [userId]
        );

        if (result.rows.length === 0) {
            return 0;
        }

        row = result.rows[0];
    }

    const notification = buildProfileUpdateNotification(row);
    const friendsResult = await pool.query(
        'SELECT friend_id FROM friends WHERE user_id = $1',
        [userId]
    );

    let notifiedCount = 0;
    for (const friend of friendsResult.rows) {
        const friendClient = clients.get(friend.friend_id);
        if (friendClient && friendClient.ws.readyState === WebSocket.OPEN) {
            friendClient.ws.send(JSON.stringify(notification));
            notifiedCount++;
        }
    }

    return notifiedCount;
};

const notifyFriendsPresenceUpdated = async (userId, userRow = null) => {
    let row = userRow;
    if (!row) {
        const result = await pool.query(
            `SELECT user_id, status, updated_at, last_seen
             FROM users
             WHERE user_id = $1`,
            [userId]
        );

        if (result.rows.length === 0) {
            return 0;
        }

        row = result.rows[0];
    }

    const notification = buildPresenceUpdateNotification(row);
    const friendsResult = await pool.query(
        'SELECT friend_id FROM friends WHERE user_id = $1',
        [userId]
    );

    let notifiedCount = 0;
    for (const friend of friendsResult.rows) {
        const friendClient = clients.get(friend.friend_id);
        if (friendClient && friendClient.ws.readyState === WebSocket.OPEN) {
            friendClient.ws.send(JSON.stringify(notification));
            notifiedCount++;
        }
    }

    return notifiedCount;
};

const assertDatabaseSchemaReady = async () => {
    const columnResult = await pool.query(
        `SELECT column_name
         FROM information_schema.columns
         WHERE table_schema = 'public'
           AND table_name = 'users'`
    );

    const existingColumns = new Set(columnResult.rows.map(row => row.column_name));
    const missingColumns = REQUIRED_USER_COLUMNS.filter(column => !existingColumns.has(column));

    if (missingColumns.length > 0) {
        throw new Error(
            `Database schema is outdated. Missing users columns: ${missingColumns.join(', ')}. ` +
            'Run server/migrations/run_migrations.sh or server\\migrations\\run_migrations.bat before starting the server.'
        );
    }

    const messageColumnResult = await pool.query(
        `SELECT column_name
         FROM information_schema.columns
         WHERE table_schema = 'public'
           AND table_name = 'messages'`
    );

    const existingMessageColumns = new Set(messageColumnResult.rows.map(row => row.column_name));
    const missingMessageColumns = REQUIRED_MESSAGE_COLUMNS.filter(column => !existingMessageColumns.has(column));
    if (missingMessageColumns.length > 0) {
        throw new Error(
            `Database schema is outdated. Missing messages columns: ${missingMessageColumns.join(', ')}. ` +
            'Run server/migrations/run_migrations.sh or server\\migrations\\run_migrations.bat before starting the server.'
        );
    }

    const migrationTableResult = await pool.query(
        `SELECT EXISTS (
            SELECT 1
            FROM information_schema.tables
            WHERE table_schema = 'public'
              AND table_name = 'schema_migrations'
        ) AS exists`
    );

    if (!migrationTableResult.rows[0].exists) {
        throw new Error(
            'Database migration history table is missing. Run server/migrations/run_migrations.sh or server\\migrations\\run_migrations.bat before starting the server.'
        );
    }

    const versionResult = await pool.query(
        `SELECT version
         FROM schema_migrations
            WHERE version IN ('001', '002', '003', '004')`
    );

    const appliedVersions = new Set(versionResult.rows.map(row => row.version));
    const missingVersions = ['001', '002', '003', '004'].filter(version => !appliedVersions.has(version));
    if (missingVersions.length > 0) {
        throw new Error(
            `Required migrations are missing: ${missingVersions.join(', ')}. ` +
            'Run server/migrations/run_migrations.sh or server\\migrations\\run_migrations.bat before starting the server.'
        );
    }

    console.log('[DB] Schema migration state verified');
};

const resolveManagedAvatarPath = (avatarValue) => {
    if (!avatarValue) {
        return null;
    }

    try {
        let pathname = '';
        if (avatarValue.startsWith('/uploads/avatars/')) {
            pathname = avatarValue;
        } else {
            const parsed = new URL(avatarValue);
            pathname = parsed.pathname || '';
        }

        if (!pathname.startsWith('/uploads/avatars/')) {
            return null;
        }

        const fileName = path.basename(pathname);
        const resolvedPath = path.resolve(avatarUploadDir, fileName);
        return resolvedPath.startsWith(avatarUploadDir) ? resolvedPath : null;
    } catch (_error) {
        return null;
    }
};

const removeManagedAvatarFile = (avatarValue) => {
    const avatarPath = resolveManagedAvatarPath(avatarValue);
    if (avatarPath && fs.existsSync(avatarPath)) {
        fs.unlink(avatarPath, (unlinkError) => {
            if (unlinkError) {
                console.warn('[Avatar] Failed to remove old avatar:', unlinkError.message);
            }
        });
    }
};

const avatarStorage = multer.diskStorage({
    destination: (_req, _file, cb) => {
        cb(null, avatarUploadDir);
    },
    filename: (req, file, cb) => {
        const extension = path.extname(file.originalname || '').toLowerCase();
        const safeExtension = ['.png', '.jpg', '.jpeg', '.gif', '.webp', '.bmp'].includes(extension)
            ? extension
            : '.png';
        const filename = `${req.user.userId}-${Date.now()}-${crypto.randomBytes(6).toString('hex')}${safeExtension}`;
        cb(null, filename);
    }
});

const avatarUpload = multer({
    storage: avatarStorage,
    limits: {
        fileSize: 5 * 1024 * 1024
    },
    fileFilter: (_req, file, cb) => {
        if ((file.mimetype || '').startsWith('image/')) {
            cb(null, true);
            return;
        }

        cb(new Error('Only image files are allowed'));
    }
});

const messageImageUpload = multer({
    storage: createUploadStorage(
        imageUploadDir,
        ['.png', '.jpg', '.jpeg', '.gif', '.webp', '.bmp'],
        'chat-image'
    ),
    limits: {
        fileSize: 15 * 1024 * 1024
    },
    fileFilter: (_req, file, cb) => {
        if ((file.mimetype || '').startsWith('image/')) {
            cb(null, true);
            return;
        }

        cb(new Error('Only image files are allowed'));
    }
});

const messageFileUpload = multer({
    storage: createUploadStorage(fileUploadDir, [], 'chat-file'),
    limits: {
        fileSize: 50 * 1024 * 1024
    }
});

// --- 中间件 ---
const authenticateToken = (req, res, next) => {
    const authHeader = req.headers['authorization'];
    const token = authHeader && authHeader.split(' ')[1];
    if (!token) return res.sendStatus(401);

    jwt.verify(token, process.env.JWT_SECRET, (err, user) => {
        if (err) return res.sendStatus(403);
        req.user = user;
        next();
    });
};

// --- HTTP API 路由 (认证) ---

// 注册
app.post('/api/auth/register', async (req, res) => {
    const { username, password } = req.body;
    try {
        const hashedPassword = await bcrypt.hash(password, 10);
        const userId = 'user_' + uuidv4().substring(0, 8);

        const result = await pool.query(
            'INSERT INTO users (user_id, username, password_hash) VALUES ($1, $2, $3) RETURNING user_id',
            [userId, username, hashedPassword]
        );

        res.status(201).json({ userId: result.rows[0].user_id, message: 'User registered' });
    } catch (err) {
        console.error(err);
        if (err.code === '23505') return res.status(400).json({ error: 'Username already exists' });
        res.status(500).json({ error: 'Internal server error' });
    }
});

// 登录
app.post('/api/auth/login', async (req, res) => {
    const { username, password } = req.body;
    console.log('[Login] Attempt:', username);

    try {
        const result = await pool.query('SELECT * FROM users WHERE username = $1', [username]);
        const user = result.rows[0];

        if (user && await bcrypt.compare(password, user.password_hash)) {
            const accessToken = jwt.sign({ userId: user.user_id, username: user.username }, process.env.JWT_SECRET);
            
            // 更新用户状态为在线
            await pool.query('UPDATE users SET status = $1, last_seen = NOW() WHERE user_id = $2', ['online', user.user_id]);
            
            console.log('[Login] Success:', user.user_id);
            res.json({ accessToken, userId: user.user_id, username: user.username });
        } else {
            console.log('[Login] Failed: Invalid credentials');
            res.status(401).json({ error: 'Invalid username or password' });
        }
    } catch (err) {
        console.error('[Login] Error:', err);
        res.status(500).json({ error: 'Internal server error' });
    }
});

// --- 受保护的 API 路由 (需要 Token) ---

app.post('/api/users/profile/avatar', authenticateToken, (req, res) => {
    avatarUpload.single('avatar')(req, res, (err) => {
        if (err instanceof multer.MulterError) {
            if (err.code === 'LIMIT_FILE_SIZE') {
                return res.status(400).json({ error: 'Avatar file is too large (max 5MB)' });
            }
            return res.status(400).json({ error: err.message });
        }

        if (err) {
            return res.status(400).json({ error: err.message || 'Avatar upload failed' });
        }

        if (!req.file) {
            return res.status(400).json({ error: 'Avatar file is required' });
        }

        const avatarPath = `/uploads/avatars/${req.file.filename}`;
        const avatarUrl = `${req.protocol}://${req.get('host')}${avatarPath}`;
        res.json({
            message: 'Avatar uploaded successfully',
            avatarPath,
            avatarUrl
        });
    });
});

app.post('/api/upload/image', authenticateToken, (req, res) => {
    messageImageUpload.single('image')(req, res, (err) => {
        if (err instanceof multer.MulterError) {
            if (err.code === 'LIMIT_FILE_SIZE') {
                return res.status(400).json({ error: 'Image file is too large (max 15MB)' });
            }
            return res.status(400).json({ error: err.message });
        }

        if (err) {
            return res.status(400).json({ error: err.message || 'Image upload failed' });
        }

        if (!req.file) {
            return res.status(400).json({ error: 'Image file is required' });
        }

        const relativePath = `/uploads/images/${req.file.filename}`;
        const fileUrl = buildManagedUploadUrl(req, relativePath);
        res.json({
            fileId: req.file.filename,
            fileUrl,
            thumbnailUrl: fileUrl,
            fileName: req.file.originalname,
            fileSize: req.file.size
        });
    });
});

app.post('/api/upload/file', authenticateToken, (req, res) => {
    messageFileUpload.single('file')(req, res, (err) => {
        if (err instanceof multer.MulterError) {
            if (err.code === 'LIMIT_FILE_SIZE') {
                return res.status(400).json({ error: 'File is too large (max 50MB)' });
            }
            return res.status(400).json({ error: err.message });
        }

        if (err) {
            return res.status(400).json({ error: err.message || 'File upload failed' });
        }

        if (!req.file) {
            return res.status(400).json({ error: 'File is required' });
        }

        const relativePath = `/uploads/files/${req.file.filename}`;
        res.json({
            fileId: req.file.filename,
            fileUrl: buildManagedUploadUrl(req, relativePath),
            fileName: req.file.originalname,
            fileSize: req.file.size
        });
    });
});

// 获取个人信息
app.get('/api/users/profile', authenticateToken, async (req, res) => {
    try {
        const result = await pool.query(
            `SELECT user_id, username, avatar, gender, region, phone, email, contact, bio, age, 
                    COALESCE(profile_version, 1) as profile_version,
                    COALESCE(profile_updated_at, created_at) as profile_updated_at,
                    created_at
             FROM users
             WHERE user_id = $1`,
            [req.user.userId]
        );
        if (result.rows.length === 0) return res.status(404).json({ error: 'User not found' });
        res.json(result.rows[0]);
    } catch (err) {
        res.status(500).json({ error: 'Internal server error' });
    }
});

app.put('/api/users/profile', authenticateToken, async (req, res) => {
    const {
        username = null,
        avatar = null,
        gender = null,
        region = null,
        phone = null,
        email = null,
        contact = null,
        bio = null,
        age = null
    } = req.body || {};

    console.log('[Profile] Update request for user:', req.user.userId);
    console.log('[Profile] Request body:', req.body);

    try {
        const existingProfileResult = await pool.query(
            'SELECT avatar FROM users WHERE user_id = $1',
            [req.user.userId]
        );

        const previousAvatar = existingProfileResult.rows.length > 0
            ? existingProfileResult.rows[0].avatar
            : null;

        // 将空字符串转换为 null，以便 COALESCE 保留原值
        const safeValue = (val) => (val === '' || val === undefined || val === null) ? null : val;
        // 年龄特殊处理：如果是数字则使用，否则转换为 null
        const safeAge = (val) => {
            if (val === undefined || val === null || val === '') return null;
            const numAge = parseInt(val);
            return (numAge > 0 && numAge <= 150) ? numAge : null;
        };

        console.log('[Profile] Safe values:', {
            username: safeValue(username),
            avatar: safeValue(avatar),
            gender: safeValue(gender),
            region: safeValue(region),
            phone: safeValue(phone),
            email: safeValue(email),
            contact: safeValue(contact),
            bio: safeValue(bio),
            age: safeAge(age)
        });

        const result = await pool.query(
            `UPDATE users
             SET username = COALESCE($1, username),
                 avatar = COALESCE($2, avatar),
                 gender = COALESCE($3, gender),
                 region = COALESCE($4, region),
                 phone = COALESCE($5, phone),
                 email = COALESCE($6, email),
                 contact = COALESCE($7, contact),
                 bio = COALESCE($8, bio),
                 age = COALESCE($9, age),
                 profile_version = profile_version + 1,
                 profile_updated_at = NOW(),
                 updated_at = NOW()
             WHERE user_id = $10
             RETURNING user_id, username, avatar, gender, region, phone, email, contact, bio, age, status, profile_version, profile_updated_at`,
            [safeValue(username), safeValue(avatar), safeValue(gender), safeValue(region),
             safeValue(phone), safeValue(email), safeValue(contact), safeValue(bio), safeAge(age), req.user.userId]
        );

        if (result.rows.length === 0) {
            console.log('[Profile] User not found:', req.user.userId);
            return res.status(404).json({ error: 'User not found' });
        }

        console.log('[Profile] Profile updated successfully:', result.rows[0]);

        const updatedAvatar = result.rows[0].avatar || '';
        if (previousAvatar && previousAvatar !== updatedAvatar) {
            removeManagedAvatarFile(previousAvatar);
        }

        // 通过 WebSocket 通知所有在线好友：用户信息已更新
        const profileUpdateNotification = buildProfileUpdateNotification(result.rows[0]);

        console.log('[Profile] Sending profile update notification:', profileUpdateNotification);

        // 获取所有好友的 ID
        const friendsResult = await pool.query(
            'SELECT friend_id FROM friends WHERE user_id = $1',
            [req.user.userId]
        );

        let notifiedCount = 0;
        let onlineCount = 0;
        friendsResult.rows.forEach(row => {
            const friendClient = clients.get(row.friend_id);
            if (friendClient && friendClient.ws.readyState === WebSocket.OPEN) {
                friendClient.ws.send(JSON.stringify(profileUpdateNotification));
                notifiedCount++;
                onlineCount++;
                console.log('[Profile] Sent update to friend:', row.friend_id);
            } else {
                console.log('[Profile] Friend', row.friend_id, 'is offline or not connected');
            }
        });

        console.log(`[Profile] Notified ${notifiedCount} online friends about profile update (version ${result.rows[0].profile_version})`);
        
        res.json({ 
            message: 'Profile updated', 
            profile: result.rows[0],
            version: result.rows[0].profile_version
        });
    } catch (err) {
        console.error('[Profile] Error:', err);
        if (err.code === '23505') {
            return res.status(400).json({ error: 'Username already exists' });
        }
        res.status(500).json({ error: 'Internal server error' });
    }
});

// 搜索用户
app.get('/api/users/search', authenticateToken, async (req, res) => {
    const { q } = req.query;
    try {
        const result = await pool.query(
            'SELECT user_id, username FROM users WHERE username ILIKE $1 OR user_id = $2 LIMIT 20',
            [`%${q}%`, q]
        );
        res.json(result.rows);
    } catch (err) {
        res.status(500).json({ error: 'Internal server error' });
    }
});

// 获取好友列表
app.get('/api/friends', authenticateToken, async (req, res) => {
    try {
        const result = await pool.query(
            `SELECT f.friend_id as user_id,
                    u.username,
                    u.avatar,
                    u.gender,
                    u.region,
                    u.bio as signature,
                    u.status,
                    u.created_at,
                    COALESCE(u.age, EXTRACT(YEAR FROM AGE(NOW(), u.created_at)) + 18) as age
             FROM friends f
             JOIN users u ON f.friend_id = u.user_id
             WHERE f.user_id = $1`,
            [req.user.userId]
        );

        // 格式化返回数据
        const friends = result.rows.map(row => {
            // 根据 gender 字段设置 isMale
            let isMale = true; // 默认值
            if (row.gender === '女') {
                isMale = false;
            } else if (row.gender === '男') {
                isMale = true;
            }

            // 使用数据库返回的年龄（已在上游 SQL 中计算）
            const age = row.age ? parseInt(row.age) : 25;

            return {
                user_id: row.user_id,
                username: row.username || '未知用户',
                avatar: row.avatar || '',
                status: row.status || 'offline',
                signature: row.signature || '',
                isMale: isMale,
                age: age,
                region: row.region || ''
            };
        });

        console.log('[Friends] Loaded', friends.length, 'friends for user', req.user.userId);
        res.json(friends);
    } catch (err) {
        console.error('[Friends] Error:', err);
        res.status(500).json({ error: 'Internal server error: ' + err.message });
    }
});

// 获取好友资料更新（用于上线后拉取更新）
app.get('/api/friends/updates/since', authenticateToken, async (req, res) => {
    try {
        const { since, versions } = req.query;
        
        // 获取所有好友 ID
        const friendsResult = await pool.query(
            'SELECT friend_id FROM friends WHERE user_id = $1',
            [req.user.userId]
        );
        
        if (friendsResult.rows.length === 0) {
            return res.json({ updates: [] });
        }
        
        const friendIds = friendsResult.rows.map(r => r.friend_id);
        
        // 查询自指定时间以来有更新的好友
        let updates = [];
        if (since) {
            const updatesResult = await pool.query(
                `SELECT u.user_id, u.username, u.avatar, u.gender, u.region,
                        u.bio as signature, u.age, u.status, u.profile_version, u.profile_updated_at
                 FROM users u
                 WHERE u.user_id = ANY($1)
                   AND u.profile_updated_at > $2`,
                [friendIds, new Date(since)]
            );
            updates = updatesResult.rows.map(row => ({
                user_id: row.user_id,
                username: row.username,
                avatar: row.avatar,
                gender: row.gender,
                region: row.region,
                signature: row.signature,
                age: row.age,
                status: row.status || 'offline',
                version: row.profile_version,
                updatedAt: row.profile_updated_at
            }));
        }
        
        // 或者根据版本号查询（更精确）
        if (versions) {
            try {
                const versionMap = JSON.parse(versions);
                const outdatedFriends = [];
                
                for (const [userId, clientVersion] of Object.entries(versionMap)) {
                    if (friendIds.includes(userId)) {
                        const result = await pool.query(
                            'SELECT profile_version FROM users WHERE user_id = $1 AND profile_version > $2',
                            [userId, clientVersion]
                        );
                        if (result.rows.length > 0) {
                            outdatedFriends.push(userId);
                        }
                    }
                }
                
                if (outdatedFriends.length > 0) {
                    const detailedUpdates = await pool.query(
                        `SELECT u.user_id, u.username, u.avatar, u.gender, u.region,
                                u.bio as signature, u.age, u.status, u.profile_version, u.profile_updated_at
                         FROM users u
                         WHERE u.user_id = ANY($1)`,
                        [outdatedFriends]
                    );

                    // 合并更新结果
                    const updateMap = new Map(updates.map(u => [u.user_id, u]));
                    detailedUpdates.rows.forEach(row => {
                        updateMap.set(row.user_id, {
                            user_id: row.user_id,
                            username: row.username,
                            avatar: row.avatar,
                            gender: row.gender,
                            region: row.region,
                            signature: row.signature,
                            age: row.age,
                            status: row.status || 'offline',
                            version: row.profile_version,
                            updatedAt: row.profile_updated_at
                        });
                    });
                    updates = Array.from(updateMap.values());
                }
            } catch (e) {
                console.error('[Friends Updates] Failed to parse versions:', e);
            }
        }
        
        console.log('[Friends Updates] Found', updates.length, 'updated friends');
        res.json({ updates });
    } catch (err) {
        console.error('[Friends Updates] Error:', err);
        res.status(500).json({ error: 'Internal server error: ' + err.message });
    }
});

// 发送好友申请
app.post('/api/friends/request', authenticateToken, async (req, res) => {
    const { toUserId } = req.body;
    console.log('=== [FriendRequest] POST /api/friends/request ===');
    console.log('[FriendRequest] From user:', req.user.userId, req.user.username);
    console.log('[FriendRequest] To user:', toUserId);
    console.log('[FriendRequest] Request body:', req.body);

    try {
        // 先检查是否已经是好友
        console.log('[FriendRequest] Checking if already friends...');
        const friendCheck = await pool.query(
            'SELECT 1 FROM friends WHERE (user_id = $1 AND friend_id = $2) OR (user_id = $2 AND friend_id = $1)',
            [req.user.userId, toUserId]
        );
        console.log('[FriendRequest] Friend check result:', friendCheck.rows.length);
        if (friendCheck.rows.length > 0) {
            return res.status(400).json({ message: 'Already friends' });
        }

        // 检查是否已有待处理的申请
        console.log('[FriendRequest] Checking existing requests...');
        const existingRequest = await pool.query(
            'SELECT request_id, status FROM friend_requests WHERE (from_id = $1 AND to_id = $2) OR (from_id = $2 AND to_id = $1)',
            [req.user.userId, toUserId]
        );
        console.log('[FriendRequest] Existing request count:', existingRequest.rows.length);
        if (existingRequest.rows.length > 0) {
            const reqStatus = existingRequest.rows[0].status;
            console.log('[FriendRequest] Existing request status:', reqStatus);
            if (reqStatus === 'pending') {
                console.log('[FriendRequest] Request already pending, returning 400');
                return res.status(400).json({ message: 'Friend request already pending' });
            } else if (reqStatus === 'accepted') {
                console.log('[FriendRequest] Already friends, returning 400');
                return res.status(400).json({ message: 'Already friends' });
            } else {
                // rejected - 允许重新发送
                console.log('[FriendRequest] Previous request was rejected, allowing new request');
            }
        }

        console.log('[FriendRequest] Inserting new request...');
        const result = await pool.query(
            'INSERT INTO friend_requests (from_id, to_id, status) VALUES ($1, $2, $3) RETURNING request_id',
            [req.user.userId, toUserId, 'pending']
        );

        const requestId = result.rows[0]?.request_id;
        console.log('[FriendRequest] Created request ID:', requestId);

        // 通过 WebSocket 通知对方
        const targetClient = clients.get(toUserId);
        console.log('[FriendRequest] Target client status:', targetClient ? 'ONLINE' : 'OFFLINE');

        if (targetClient && targetClient.ws.readyState === WebSocket.OPEN) {
            const notification = {
                type: 'friend_request',
                requestId: requestId,
                fromId: req.user.userId,
                fromName: req.user.username
            };
            targetClient.ws.send(JSON.stringify(notification));
            console.log('[FriendRequest] ✓ Notified user', toUserId, 'about request from', req.user.userId);
            console.log('[FriendRequest] Notification payload:', notification);
        } else {
            console.log('[FriendRequest] ✗ User', toUserId, 'is not online, notification skipped');
        }

        res.status(201).json({ message: 'Friend request sent', requestId });
        console.log('[FriendRequest] Response sent:', { message: 'Friend request sent', requestId });
    } catch (err) {
        console.error('[FriendRequest] Error:', err.message);
        console.error('[FriendRequest] Error detail:', err);
        res.status(400).json({ error: 'Bad request: ' + err.message });
    }
});

// 处理好友申请
app.put('/api/friends/requests/:id', authenticateToken, async (req, res) => {
    const requestId = req.params.id;
    const { status } = req.body; // 'accepted' or 'rejected'

    try {
        const transactionResult = await withTransaction(async (client) => {
            const request = await client.query('SELECT * FROM friend_requests WHERE request_id = $1', [requestId]);

            if (request.rows.length === 0) {
                return { statusCode: 404, body: { error: 'Request not found' } };
            }

            const { from_id, to_id } = request.rows[0];
            if (to_id !== req.user.userId) {
                return { statusCode: 403, body: { error: 'Unauthorized' } };
            }

            await client.query('UPDATE friend_requests SET status = $1 WHERE request_id = $2', [status, requestId]);

            let conversationId = null;
            if (status === 'accepted') {
                await client.query(
                    'INSERT INTO friends (user_id, friend_id) VALUES ($1, $2), ($2, $1) ON CONFLICT DO NOTHING',
                    [from_id, to_id]
                );
                console.log('[FriendRequest] Added to friends table');

                const sortedIds = [from_id, to_id].sort();
                conversationId = 'conv_' + sortedIds[0] + '_' + sortedIds[1];

                const friendResult = await client.query(
                    'SELECT username FROM users WHERE user_id = $1',
                    [to_id]
                );
                const friendUsername = friendResult.rows[0]?.username || 'Chat';

                await client.query(
                    `INSERT INTO conversations (conversation_id, name, type, creator_id, last_message, last_message_time)
                     VALUES ($1, $2, $3, $4, $5, $6)
                     ON CONFLICT (conversation_id) DO NOTHING`,
                    [conversationId, friendUsername, 'single', from_id, '', new Date()]
                );

                await client.query(
                    `INSERT INTO conversation_members (conversation_id, user_id)
                     VALUES ($1, $2), ($1, $3)
                     ON CONFLICT DO NOTHING`,
                    [conversationId, from_id, to_id]
                );
                console.log('[FriendRequest] Ensured conversation and members:', conversationId);
            }

            return {
                statusCode: 200,
                body: { message: `Request ${status}` },
                notification: status === 'accepted'
                    ? {
                        fromId: from_id,
                        toId: to_id,
                        conversationId
                    }
                    : null
            };
        });

        if (transactionResult.statusCode !== 200) {
            return res.status(transactionResult.statusCode).json(transactionResult.body);
        }

        if (transactionResult.notification) {
            const { fromId, toId, conversationId } = transactionResult.notification;
            const targetClient = clients.get(fromId);
            if (targetClient && targetClient.ws.readyState === WebSocket.OPEN) {
                targetClient.ws.send(JSON.stringify({
                    type: 'friend_response',
                    accepted: true,
                    toId,
                    toName: req.user.username,
                    conversationId
                }));
            }
            console.log('[FriendRequest] Notified friend request accepted');
        }

        res.json({ message: `Request ${status}` });
    } catch (err) {
        console.error('[FriendRequest] Transaction failed:', err);
        res.status(500).json({ error: 'Internal server error' });
    }
});

// 获取好友申请列表（包括待处理、已接受、已拒绝）
app.get('/api/friends/requests', authenticateToken, async (req, res) => {
    try {
        const result = await pool.query(
            `SELECT fr.request_id, fr.from_id, fr.to_id, fr.status, fr.created_at, 
                    u.username as from_username
             FROM friend_requests fr
             JOIN users u ON fr.from_id = u.user_id
             WHERE fr.to_id = $1
             ORDER BY fr.created_at DESC`,
            [req.user.userId]
        );
        res.json(result.rows);
    } catch (err) {
        console.error('Get friend requests error:', err);
        res.status(500).json({ error: 'Internal server error' });
    }
});

// 获取待处理的好友申请数量
app.get('/api/friends/requests/pending/count', authenticateToken, async (req, res) => {
    try {
        const result = await pool.query(
            'SELECT COUNT(*) as count FROM friend_requests WHERE to_id = $1 AND status = $2',
            [req.user.userId, 'pending']
        );
        res.json({ count: parseInt(result.rows[0].count) });
    } catch (err) {
        res.status(500).json({ error: 'Internal server error' });
    }
});

// 获取单个好友详情（用于点击时刷新最新信息）
app.get('/api/friends/:friendId', authenticateToken, async (req, res) => {
    try {
        const friendId = req.params.friendId;
        
        // 验证是否为好友关系
        const friendCheck = await pool.query(
            'SELECT 1 FROM friends WHERE (user_id = $1 AND friend_id = $2) OR (user_id = $2 AND friend_id = $1)',
            [req.user.userId, friendId]
        );
        
        if (friendCheck.rows.length === 0) {
            return res.status(404).json({ error: 'Friend not found or not your friend' });
        }
        
        const result = await pool.query(
            `SELECT u.user_id,
                    u.username,
                    u.avatar,
                    u.gender,
                    u.region,
                    u.bio as signature,
                    u.status,
                    u.created_at,
                    COALESCE(u.age, EXTRACT(YEAR FROM AGE(NOW(), u.created_at)) + 18) as age,
                    COALESCE(u.profile_version, 1) as profile_version,
                    COALESCE(u.profile_updated_at, u.created_at) as profile_updated_at
             FROM users u
             WHERE u.user_id = $1`,
            [friendId]
        );
        
        if (result.rows.length === 0) {
            return res.status(404).json({ error: 'User not found' });
        }
        
        const row = result.rows[0];
        
        // 根据 gender 字段设置 isMale
        let isMale = true;
        if (row.gender === '女') {
            isMale = false;
        } else if (row.gender === '男') {
            isMale = true;
        }
        
        const age = row.age ? parseInt(row.age) : 25;
        
        const friendInfo = {
            user_id: row.user_id,
            username: row.username || '未知用户',
            avatar: row.avatar || '',
            status: row.status || 'offline',
            signature: row.signature || '',
            isMale: isMale,
            age: age,
            region: row.region || '',
            version: row.profile_version,
            updatedAt: row.profile_updated_at
        };
        
        console.log('[Friend Detail] Loaded detail for:', friendId, 'version:', friendInfo.version);
        res.json(friendInfo);
    } catch (err) {
        console.error('[Friend Detail] Error:', err);
        res.status(500).json({ error: 'Internal server error: ' + err.message });
    }
});

// 获取所有对话
app.get('/api/conversations', authenticateToken, async (req, res) => {
    try {
        const result = await pool.query(
            `SELECT c.conversation_id, c.name, c.type, c.creator_id, c.last_message,
                    c.last_message_time, c.created_at, c.updated_at,
                    u.user_id as other_user_id, u.username as other_username, u.avatar as other_avatar
             FROM conversations c
             JOIN conversation_members cm ON c.conversation_id = cm.conversation_id
             LEFT JOIN conversation_members cm_other ON c.conversation_id = cm_other.conversation_id
                 AND cm_other.user_id != $1
             LEFT JOIN users u ON cm_other.user_id = u.user_id
             WHERE cm.user_id = $1
             ORDER BY c.last_message_time DESC NULLS LAST`,
            [req.user.userId]
        );

        // 处理结果：对于单聊，使用对方用户名作为会话名称
        const conversations = result.rows.map(row => {
            if (row.type === 'single') {
                // 优先使用 other_username，如果没有则使用数据库中存储的 name
                const displayName = row.other_username || row.name || 'Chat';
                return {
                    ...row,
                    name: displayName,
                    avatar: row.other_avatar || ''
                };
            }
            return row;
        });

        res.json(conversations);
    } catch (err) {
        console.error('Get conversations error:', err);
        res.status(500).json({ error: 'Internal server error' });
    }
});

// 获取消息记录（带时间戳转换）

// 获取消息记录
app.get('/api/messages/:conversationId', authenticateToken, async (req, res) => {
    const { conversationId } = req.params;
    const { limit = 50, offset = 0 } = req.query;
    try {
        const result = await pool.query(
            'SELECT * FROM messages WHERE conversation_id = $1 ORDER BY timestamp DESC LIMIT $2 OFFSET $3',
            [conversationId, limit, offset]
        );
        // 将秒级时间戳转换为毫秒级返回给客户端
        const messages = result.rows.reverse().map(row => ({
            ...row,
            status: row.status ?? 1,
            recalled: row.recalled === true,
            type: messageTypeToClientValue(row.type),
            timestamp: row.timestamp * 1000  // 秒 -> 毫秒
        }));
        res.json(messages);
    } catch (err) {
        res.status(500).json({ error: 'Internal server error' });
    }
});

// 标记消息为已读
app.put('/api/messages/read', authenticateToken, async (req, res) => {
    const { conversationId } = req.body;
    try {
        await pool.query(
            'UPDATE messages SET is_read = TRUE WHERE conversation_id = $1 AND sender_id != $2',
            [conversationId, req.user.userId]
        );
        res.json({ message: 'Messages marked as read' });
    } catch (err) {
        res.status(500).json({ error: 'Internal server error' });
    }
});

// --- 密码重置 API ---

// 生成重置码（发送到邮箱的简化版，直接返回）
app.post('/api/auth/forgot-password', async (req, res) => {
    const { username } = req.body;
    try {
        const userResult = await pool.query('SELECT user_id FROM users WHERE username = $1', [username]);
        if (userResult.rows.length === 0) {
            return res.status(404).json({ error: 'User not found' });
        }

        const userId = userResult.rows[0].user_id;
        const resetCode = crypto.randomBytes(16).toString('hex').substring(0, 8).toUpperCase();
        const expiresAt = new Date(Date.now() + 15 * 60 * 1000); // 15 分钟有效

        // 保存或更新重置码
        await pool.query(
            `INSERT INTO password_resets (reset_code, user_id, expires_at) 
             VALUES ($1, $2, $3) 
             ON CONFLICT (reset_code) 
             DO UPDATE SET user_id = $2, expires_at = $3, created_at = NOW()`,
            [resetCode, userId, expiresAt]
        );

        // 生产环境应发送邮件，这里直接返回重置码供调试
        res.json({ 
            message: 'Password reset code generated', 
            resetCode, // 调试用，生产环境应移除
            expiresAt: expiresAt.toISOString()
        });
    } catch (err) {
        console.error('Forgot password error:', err);
        res.status(500).json({ error: 'Internal server error' });
    }
});

// 验证重置码并设置新密码
app.post('/api/auth/reset-password', async (req, res) => {
    const { resetCode, newPassword } = req.body;
    try {
        const resetResult = await pool.query(
            'SELECT user_id, expires_at FROM password_resets WHERE reset_code = $1',
            [resetCode.toUpperCase()]
        );

        if (resetResult.rows.length === 0) {
            return res.status(400).json({ error: 'Invalid reset code' });
        }

        const { user_id, expires_at } = resetResult.rows[0];
        if (new Date(expires_at) < new Date()) {
            return res.status(400).json({ error: 'Reset code expired' });
        }

        const hashedPassword = await bcrypt.hash(newPassword, 10);
        await pool.query('UPDATE users SET password_hash = $1, updated_at = NOW() WHERE user_id = $2', [hashedPassword, user_id]);
        await pool.query('DELETE FROM password_resets WHERE reset_code = $1', [resetCode]);

        res.json({ message: 'Password reset successful' });
    } catch (err) {
        console.error('Reset password error:', err);
        res.status(500).json({ error: 'Internal server error' });
    }
});

// 管理员直接设置用户密码
app.put('/api/users/:userId/password', authenticateToken, async (req, res) => {
    const { userId } = req.params;
    const { newPassword } = req.body;
    try {
        const hashedPassword = await bcrypt.hash(newPassword, 10);
        await pool.query('UPDATE users SET password_hash = $1, updated_at = NOW() WHERE user_id = $2', [hashedPassword, userId]);
        res.json({ message: 'Password updated successfully' });
    } catch (err) {
        console.error('Admin set password error:', err);
        res.status(500).json({ error: 'Internal server error' });
    }
});

// --- 下载 API ---

// 用户登出 API
app.post('/api/auth/logout', authenticateToken, async (req, res) => {
    try {
        // 更新用户状态为离线
        const result = await pool.query(
            `UPDATE users
             SET status = $1, last_seen = NOW(), updated_at = NOW()
             WHERE user_id = $2
             RETURNING user_id, status, updated_at, last_seen`,
            ['offline', req.user.userId]
        );
        if (result.rows.length > 0) {
            await notifyFriendsPresenceUpdated(req.user.userId, result.rows[0]);
        }
        console.log('[Logout] User', req.user.userId, 'logged out');
        res.json({ message: 'Logout successful' });
    } catch (err) {
        console.error('[Logout] Error:', err);
        res.status(500).json({ error: 'Internal server error' });
    }
});

// 获取可用的下载文件列表
app.get('/api/downloads', (req, res) => {
    const downloadsDir = path.join(__dirname, '..', 'web', 'downloads');

    if (!fs.existsSync(downloadsDir)) {
        return res.json({ files: [], message: 'No downloads available' });
    }

    const files = fs.readdirSync(downloadsDir).map(file => {
        const filePath = path.join(downloadsDir, file);
        const stats = fs.statSync(filePath);
        return {
            name: file,
            size: stats.size,
            sizeFormatted: formatFileSize(stats.size),
            downloadUrl: `/downloads/${file}`
        };
    });

    res.json({ files });
});

// 格式化文件大小
function formatFileSize(bytes) {
    const units = ['B', 'KB', 'MB', 'GB'];
    let size = bytes;
    let unitIndex = 0;
    while (size >= 1024 && unitIndex < units.length - 1) {
        size /= 1024;
        unitIndex++;
    }
    return `${size.toFixed(2)} ${units[unitIndex]}`;
}

// 下载文件
app.get('/api/download/:filename', (req, res) => {
    const filename = req.params.filename;
    const filePath = path.join(__dirname, '..', 'web', 'downloads', filename);
    
    // 安全检查：防止目录遍历攻击
    const normalizedPath = path.normalize(filePath);
    const downloadsDir = path.join(__dirname, '..', 'web', 'downloads');
    
    if (!normalizedPath.startsWith(downloadsDir)) {
        return res.status(403).json({ error: 'Access denied' });
    }
    
    if (!fs.existsSync(filePath)) {
        return res.status(404).json({ error: 'File not found' });
    }
    
    res.download(filePath, filename);
});

// --- WebSocket 处理 ---

const clients = new Map(); // userId -> { ws, username }

server.on('upgrade', (request, socket, head) => {
    // 这里可以处理 WebSocket 握手时的鉴权，但为了简单起见，我们放在消息里处理第一个 'auth' 包
    wss.handleUpgrade(request, socket, head, (ws) => {
        wss.emit('connection', ws, request);
    });
});

wss.on('connection', (ws) => {
    let currentUserId = null;

    ws.on('message', async (message) => {
        let data;
        try {
            data = JSON.parse(message);
        } catch (e) { return; }

        const { type } = data;

        // 1. 认证
        if (type === 'auth') {
            const { token, lastSyncTime, friendVersions } = data;
            try {
                const decoded = jwt.verify(token, process.env.JWT_SECRET);
                currentUserId = decoded.userId;
                clients.set(currentUserId, { ws, username: decoded.username });

                const statusResult = await pool.query(
                    `UPDATE users
                     SET status = 'online', last_seen = NOW(), updated_at = NOW()
                     WHERE user_id = $1
                     RETURNING user_id, username, avatar, gender, region,
                               bio, age, status, profile_version, profile_updated_at, updated_at`,
                    [currentUserId]
                );

                if (statusResult.rows.length > 0) {
                    const notifiedCount = await notifyFriendsPresenceUpdated(currentUserId, statusResult.rows[0]);
                    console.log(`[WebSocket] Broadcast online status for ${currentUserId} to ${notifiedCount} friends`);
                }

                ws.send(JSON.stringify({ 
                    type: 'auth_success', 
                    userId: currentUserId 
                }));
                console.log(`[WebSocket] User ${decoded.username} (${decoded.userId}) authenticated`);
                console.log(`[WebSocket] Total online users: ${clients.size}`);
                
                // 认证成功后，推送好友更新（如果有）
                if (lastSyncTime || friendVersions) {
                    // 异步查询好友更新
                    (async () => {
                        try {
                            const friendsResult = await pool.query(
                                'SELECT friend_id FROM friends WHERE user_id = $1',
                                [currentUserId]
                            );
                            
                            if (friendsResult.rows.length === 0) return;
                            
                            const friendIds = friendsResult.rows.map(r => r.friend_id);
                            let updates = [];
                            
                            // 根据时间查询更新
                            if (lastSyncTime) {
                                const updatesResult = await pool.query(
                                        `SELECT u.user_id, u.username, u.avatar, u.gender, u.region,
                                            u.bio as signature, u.age, u.status, u.profile_version, u.profile_updated_at
                                     FROM users u
                                     WHERE u.user_id = ANY($1) AND u.profile_updated_at > $2`,
                                    [friendIds, new Date(lastSyncTime)]
                                );
                                
                                updates = updatesResult.rows.map(row => ({
                                    user_id: row.user_id,
                                    username: row.username,
                                    avatar: row.avatar,
                                    gender: row.gender,
                                    region: row.region,
                                    signature: row.signature,
                                    age: row.age,
                                    status: row.status,
                                    version: row.profile_version,
                                    updatedAt: row.profile_updated_at
                                }));
                            }
                            
                            // 根据版本号查询更新（更精确）
                            if (friendVersions) {
                                const versionMap = typeof friendVersions === 'string' ? 
                                    JSON.parse(friendVersions) : friendVersions;
                                
                                for (const [userId, clientVersion] of Object.entries(versionMap)) {
                                    if (friendIds.includes(userId)) {
                                        const result = await pool.query(
                                            'SELECT profile_version FROM users WHERE user_id = $1 AND profile_version > $2',
                                            [userId, clientVersion]
                                        );
                                        if (result.rows.length > 0 && !updates.find(u => u.user_id === userId)) {
                                            const detail = await pool.query(
                                                `SELECT u.user_id, u.username, u.avatar, u.gender, u.region,
                                                    u.bio as signature, u.age, u.status, u.profile_version, u.profile_updated_at
                                                 FROM users u WHERE u.user_id = $1`,
                                                [userId]
                                            );
                                            if (detail.rows.length > 0) {
                                                const row = detail.rows[0];
                                                updates.push({
                                                    user_id: row.user_id,
                                                    username: row.username,
                                                    avatar: row.avatar,
                                                    gender: row.gender,
                                                    region: row.region,
                                                    signature: row.signature,
                                                    age: row.age,
                                                    status: row.status,
                                                    version: row.profile_version,
                                                    updatedAt: row.profile_updated_at
                                                });
                                            }
                                        }
                                    }
                                }
                            }
                            
                            if (updates.length > 0) {
                                ws.send(JSON.stringify({
                                    type: 'friends_updates',
                                    updates: updates
                                }));
                                console.log(`[WebSocket] Sent ${updates.length} friend updates to ${currentUserId}`);
                            }
                        } catch (e) {
                            console.error('[WebSocket] Failed to fetch friend updates:', e);
                        }
                    })();
                }
            } catch (err) {
                console.error('[WebSocket] Auth error:', err.message);
                ws.send(JSON.stringify({ type: 'error', message: 'Invalid token' }));
                ws.close();
            }
            return;
        }

        if (!currentUserId) return;

        // 2. 心跳
        if (type === 'heartbeat') {
            ws.send(JSON.stringify({ 
                type: 'heartbeat', 
                timestamp: Date.now(),
                latency: data.timestamp ? Date.now() - data.timestamp : 0
            }));
            return;
        }

        // 3. 消息转发与存储
        if (type === 'message') {
            const {
                conversationId,
                clientMessageId = null,
                content,
                timestamp,
                messageType = 0,
                status = 1,
                fileId = null,
                fileName = null,
                fileSize = null,
                fileUrl = null,
                thumbnailUrl = null
            } = data;
            const messageId = 'msg_' + uuidv4().substring(0, 8);
            const normalizedMessageType = normalizeMessageType(messageType);
            const normalizedStatus = Number.isFinite(Number(status)) ? Number(status) : 1;

            console.log(`[Message] Received message from ${currentUserId} to conversation ${conversationId}`);
            console.log(`[Message] Content: ${content}`);

            try {
                const timestampSeconds = Math.floor(timestamp / 1000);
                const transactionResult = await withTransaction(async (client) => {
                    let finalConversationId = conversationId;

                    const convCheck = await client.query(
                        'SELECT conversation_id FROM conversations WHERE conversation_id = $1',
                        [conversationId]
                    );

                    if (convCheck.rows.length === 0) {
                        console.log('[Message] Conversation not found, creating new one...');

                        const targetUserId = conversationId.startsWith('user_') ? conversationId : null;
                        if (!targetUserId) {
                            throw new Error('Conversation not found');
                        }

                        const sortedIds = [currentUserId, targetUserId].sort();
                        finalConversationId = 'conv_' + sortedIds[0] + '_' + sortedIds[1];

                        await client.query(
                            `INSERT INTO conversations (conversation_id, name, type, creator_id, last_message, last_message_time)
                             VALUES ($1, $2, $3, $4, $5, $6)
                             ON CONFLICT (conversation_id) DO NOTHING`,
                            [finalConversationId, 'Chat', 'single', currentUserId, '', new Date()]
                        );

                        await client.query(
                            `INSERT INTO conversation_members (conversation_id, user_id)
                             VALUES ($1, $2), ($1, $3)
                             ON CONFLICT DO NOTHING`,
                            [finalConversationId, currentUserId, targetUserId]
                        );

                        console.log('[Message] Ensured conversation exists:', finalConversationId);
                    }

                    await client.query(
                        `INSERT INTO messages (
                            message_id, conversation_id, sender_id, content, type, status, timestamp,
                            file_id, file_name, file_size, file_url, thumbnail_url
                         ) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12)`,
                        [
                            messageId,
                            finalConversationId,
                            currentUserId,
                            content,
                            normalizedMessageType,
                            normalizedStatus,
                            timestampSeconds,
                            fileId,
                            fileName,
                            fileSize,
                            fileUrl,
                            thumbnailUrl
                        ]
                    );

                    await client.query(
                        `UPDATE conversations
                         SET last_message = $1, last_message_time = NOW()
                         WHERE conversation_id = $2`,
                        [content, finalConversationId]
                    );

                    const membersResult = await client.query(
                        'SELECT user_id FROM conversation_members WHERE conversation_id = $1',
                        [finalConversationId]
                    );

                    return {
                        finalConversationId,
                        memberIds: membersResult.rows.map(row => row.user_id)
                    };
                });

                console.log('[Message] Saved to database');
                console.log('[Message] Updated conversation');
                console.log('[Message] Conversation members:', transactionResult.memberIds);

                // 转发消息给在线成员
                const payload = JSON.stringify({
                    type: 'message',
                    messageId,
                    conversationId: transactionResult.finalConversationId,
                    senderId: currentUserId,
                    content,
                    timestamp,
                    messageType: messageTypeToClientValue(normalizedMessageType),
                    status: normalizedStatus,
                    fileId,
                    fileName,
                    fileSize,
                    fileUrl,
                    thumbnailUrl
                });

                let sentCount = 0;
                transactionResult.memberIds.forEach(memberId => {
                    if (memberId !== currentUserId) { // 不发回给自己
                        const client = clients.get(memberId);
                        if (client && client.ws.readyState === WebSocket.OPEN) {
                            client.ws.send(payload);
                            console.log(`[Message] ✓ Forwarded to ${memberId}`);
                            sentCount++;
                        } else {
                            console.log(`[Message] ✗ User ${memberId} is offline`);
                        }
                    }
                });
                console.log(`[Message] Forwarded to ${sentCount} users`);

                ws.send(JSON.stringify({
                    type: 'message_ack',
                    success: true,
                    clientMessageId,
                    messageId,
                    conversationId: transactionResult.finalConversationId,
                    timestamp: Date.now()
                }));

            } catch (err) {
                console.error('[Message] Failed to process message:', err);
                ws.send(JSON.stringify({
                    type: 'message_ack',
                    success: false,
                    clientMessageId,
                    conversationId,
                    error: err.message || '消息发送失败'
                }));
            }
        }

        if (type === 'recall_message') {
            const { conversationId, messageId } = data;
            if (!conversationId || !messageId) {
                ws.send(JSON.stringify({
                    type: 'message_recalled',
                    success: false,
                    conversationId,
                    messageId,
                    error: '撤回参数无效'
                }));
                return;
            }

            try {
                const messageResult = await pool.query(
                    `SELECT message_id, sender_id, timestamp, recalled
                     FROM messages
                     WHERE message_id = $1 AND conversation_id = $2`,
                    [messageId, conversationId]
                );

                if (messageResult.rows.length === 0) {
                    ws.send(JSON.stringify({
                        type: 'message_recalled',
                        success: false,
                        conversationId,
                        messageId,
                        error: '消息不存在'
                    }));
                    return;
                }

                const messageRow = messageResult.rows[0];
                if (messageRow.sender_id !== currentUserId) {
                    ws.send(JSON.stringify({
                        type: 'message_recalled',
                        success: false,
                        conversationId,
                        messageId,
                        error: '只能撤回自己发送的消息'
                    }));
                    return;
                }

                if (messageRow.recalled) {
                    ws.send(JSON.stringify({
                        type: 'message_recalled',
                        success: true,
                        conversationId,
                        messageId
                    }));
                    return;
                }

                const nowSeconds = Math.floor(Date.now() / 1000);
                if (nowSeconds - Number(messageRow.timestamp) > 120) {
                    ws.send(JSON.stringify({
                        type: 'message_recalled',
                        success: false,
                        conversationId,
                        messageId,
                        error: '超过 2 分钟，无法撤回'
                    }));
                    return;
                }

                await pool.query(
                    `UPDATE messages
                     SET recalled = TRUE
                     WHERE message_id = $1`,
                    [messageId]
                );

                const membersResult = await pool.query(
                    'SELECT user_id FROM conversation_members WHERE conversation_id = $1',
                    [conversationId]
                );

                const payload = JSON.stringify({
                    type: 'message_recalled',
                    success: true,
                    conversationId,
                    messageId
                });

                membersResult.rows.forEach(row => {
                    const client = clients.get(row.user_id);
                    if (client && client.ws.readyState === WebSocket.OPEN) {
                        client.ws.send(payload);
                    }
                });
                return;
            } catch (err) {
                console.error('[Recall] Failed to recall message:', err);
                ws.send(JSON.stringify({
                    type: 'message_recalled',
                    success: false,
                    conversationId,
                    messageId,
                    error: err.message || '消息撤回失败'
                }));
                return;
            }
        }
    });

    ws.on('close', async () => {
        if (currentUserId) {
            // 更新用户状态为离线
            try {
                const statusResult = await pool.query(
                    `UPDATE users
                     SET status = $1, last_seen = NOW(), updated_at = NOW()
                     WHERE user_id = $2
                     RETURNING user_id, username, avatar, gender, region,
                               bio, age, status, profile_version, profile_updated_at, updated_at`,
                    ['offline', currentUserId]
                );
                console.log(`[WebSocket] User ${currentUserId} status updated to offline`);

                if (statusResult.rows.length > 0) {
                    const notifiedCount = await notifyFriendsPresenceUpdated(currentUserId, statusResult.rows[0]);
                    console.log(`[WebSocket] Broadcast offline status for ${currentUserId} to ${notifiedCount} friends`);
                }
            } catch (err) {
                console.error('[WebSocket] Failed to update user status:', err);
            }
            
            clients.delete(currentUserId);
            console.log(`User ${currentUserId} disconnected`);
        }
    });
});

const PORT = process.env.PORT || 8080;

// 启动服务器前测试数据库连接
async function startServer() {
    try {
        // 测试数据库连接
        const testQuery = await pool.query('SELECT NOW() as db_time');
        console.log('✅ PostgreSQL connected:', testQuery.rows[0].db_time);
        await assertDatabaseSchemaReady();
        
        // 启动 HTTP 服务器
        server.listen(PORT, () => {
            console.log(`Server running on http://localhost:${PORT}`);
            console.log(`✅ Database: ${process.env.DB_NAME}@${process.env.DB_HOST}:${process.env.DB_PORT}`);
        });
    } catch (err) {
        console.error('❌ Database connection failed:', err.message);
        console.error('Please check:');
        console.error('  1. PostgreSQL service is running');
        console.error('  2. Database credentials in .env file');
        console.error('  3. Database exists and server/init_db.sql has been applied');
        console.error('  4. Required migrations have been applied via server/migrations/run_migrations.sh or server\\migrations\\run_migrations.bat');
        process.exit(1);
    }
}

startServer();
