/**
 * ESChat - 前端 JavaScript 逻辑
 */

// API 配置 - 根据实际部署环境修改
const API_BASE_URL = window.location.hostname === 'localhost' 
    ? 'http://localhost:8080/api' 
    : '/api';

// ===================================
// 模态框控制
// ===================================

function showLogin() {
    closeModal('registerModal');
    document.getElementById('loginModal').classList.add('active');
}

function showRegister() {
    closeModal('loginModal');
    document.getElementById('registerModal').classList.add('active');
}

function closeModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
        modal.classList.remove('active');
    }
}

// 点击模态框外部关闭
document.querySelectorAll('.modal').forEach(modal => {
    modal.addEventListener('click', (e) => {
        if (e.target === modal) {
            modal.classList.remove('active');
        }
    });
});

// ESC 键关闭模态框
document.addEventListener('keydown', (e) => {
    if (e.key === 'Escape') {
        document.querySelectorAll('.modal.active').forEach(modal => {
            modal.classList.remove('active');
        });
    }
});

// ===================================
// 表单处理
// ===================================

async function handleLogin(event) {
    event.preventDefault();
    
    const form = event.target;
    const username = form.username.value.trim();
    const password = form.password.value;
    const remember = form.remember.checked;
    
    const submitBtn = form.querySelector('button[type="submit"]');
    const originalText = submitBtn.textContent;
    submitBtn.textContent = '登录中...';
    submitBtn.disabled = true;
    
    try {
        const response = await fetch(`${API_BASE_URL}/auth/login`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ username, password }),
        });
        
        const data = await response.json();
        
        if (response.ok && data.token) {
            showToast('登录成功！', 'success');
            
            // 保存 Token
            localStorage.setItem('token', data.token);
            if (remember) {
                localStorage.setItem('rememberedUsername', username);
            }
            
            // 关闭模态框
            closeModal('loginModal');
            
            // 更新 UI
            updateAuthUI(true, data.user || { username });
            
            // 保存密码（如果启用了记住我）
            if (remember) {
                localStorage.setItem('rememberedPassword', password);
            }
        } else {
            showToast(data.error || '登录失败，请检查用户名和密码', 'error');
        }
    } catch (error) {
        console.error('Login error:', error);
        showToast('网络错误，请稍后重试', 'error');
    } finally {
        submitBtn.textContent = originalText;
        submitBtn.disabled = false;
    }
}

async function handleRegister(event) {
    event.preventDefault();
    
    const form = event.target;
    const username = form.username.value.trim();
    const password = form.password.value;
    const confirmPassword = form.confirmPassword.value;
    
    // 验证密码
    if (password !== confirmPassword) {
        showToast('两次输入的密码不一致', 'error');
        return;
    }
    
    // 验证密码强度
    if (password.length < 6) {
        showToast('密码长度至少为 6 位', 'error');
        return;
    }
    
    const submitBtn = form.querySelector('button[type="submit"]');
    const originalText = submitBtn.textContent;
    submitBtn.textContent = '注册中...';
    submitBtn.disabled = true;
    
    try {
        const response = await fetch(`${API_BASE_URL}/auth/register`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ username, password }),
        });
        
        const data = await response.json();
        
        if (response.ok && data.token) {
            showToast('注册成功！', 'success');
            
            // 保存 Token
            localStorage.setItem('token', data.token);
            localStorage.setItem('rememberedUsername', username);
            localStorage.setItem('rememberedPassword', password);
            
            // 关闭模态框
            closeModal('registerModal');
            
            // 更新 UI
            updateAuthUI(true, data.user || { username });
        } else {
            showToast(data.error || '注册失败，请稍后重试', 'error');
        }
    } catch (error) {
        console.error('Register error:', error);
        showToast('网络错误，请稍后重试', 'error');
    } finally {
        submitBtn.textContent = originalText;
        submitBtn.disabled = false;
    }
}

// ===================================
// UI 更新
// ===================================

function updateAuthUI(isLoggedIn, user = {}) {
    const navActions = document.querySelector('.nav-actions');
    
    if (isLoggedIn) {
        navActions.innerHTML = `
            <span class="user-welcome">欢迎，${user.username || '用户'}</span>
            <button class="btn btn-ghost" onclick="handleLogout()">退出</button>
        `;
    } else {
        navActions.innerHTML = `
            <button class="btn btn-ghost" onclick="showLogin()">登录</button>
            <button class="btn btn-primary" onclick="showRegister()">注册</button>
        `;
    }
}

function handleLogout() {
    localStorage.removeItem('token');
    localStorage.removeItem('rememberedUsername');
    localStorage.removeItem('rememberedPassword');
    showToast('已退出登录', 'info');
    updateAuthUI(false);
}

// ===================================
// Toast 提示
// ===================================

let toastTimeout;

function showToast(message, type = 'info') {
    const toast = document.getElementById('toast');
    toast.textContent = message;
    toast.className = `toast ${type} show`;
    
    clearTimeout(toastTimeout);
    toastTimeout = setTimeout(() => {
        toast.classList.remove('show');
    }, 3000);
}

// ===================================
// 导航栏滚动效果
// ===================================

window.addEventListener('scroll', () => {
    const navbar = document.querySelector('.navbar');
    if (window.scrollY > 50) {
        navbar.classList.add('scrolled');
    } else {
        navbar.classList.remove('scrolled');
    }
});

// ===================================
// 移动端菜单
// ===================================

function toggleMobileMenu() {
    const navLinks = document.querySelector('.nav-links');
    const navActions = document.querySelector('.nav-actions');
    
    navLinks.classList.toggle('mobile-active');
    navActions.classList.toggle('mobile-active');
}

// ===================================
// 初始化
// ===================================

document.addEventListener('DOMContentLoaded', () => {
    // 检查登录状态
    const token = localStorage.getItem('token');
    const rememberedUsername = localStorage.getItem('rememberedUsername');
    
    if (token) {
        // 这里可以验证 Token 是否有效
        updateAuthUI(true, { username: rememberedUsername || '用户' });
    }
    
    // 预填充用户名（如果启用了记住我）
    if (rememberedUsername) {
        const loginUsername = document.getElementById('loginUsername');
        const rememberedPassword = localStorage.getItem('rememberedPassword');
        
        if (loginUsername) {
            loginUsername.value = rememberedUsername;
        }
        
        // 如果记住了密码，也填充密码
        if (rememberedPassword) {
            const loginPassword = document.getElementById('loginPassword');
            if (loginPassword) {
                loginPassword.value = rememberedPassword;
            }
            
            // 勾选记住我
            const rememberCheckbox = document.querySelector('input[name="remember"]');
            if (rememberCheckbox) {
                rememberCheckbox.checked = true;
            }
        }
    }
    
    // 平滑滚动到锚点
    document.querySelectorAll('a[href^="#"]').forEach(anchor => {
        anchor.addEventListener('click', function(e) {
            const href = this.getAttribute('href');
            if (href !== '#') {
                e.preventDefault();
                const target = document.querySelector(href);
                if (target) {
                    const offsetTop = target.offsetTop - 70;
                    window.scrollTo({
                        top: offsetTop,
                        behavior: 'smooth'
                    });
                }
            }
        });
    });
});

// ===================================
// 下载客户端
// ===================================

async function downloadClient(platform) {
    // 客户端打包文件配置
    // 将 Qt 打包好的文件夹压缩成 ZIP 文件，放入 web/downloads/ 目录
    const downloads = {
        'windows': 'ESChat-Windows.zip',
        'macos': 'ESChat-macOS.zip',
        'linux': 'ESChat-Linux.zip'
    };

    const filename = downloads[platform];
    if (!filename) {
        showToast('暂不支持该平台', 'error');
        return;
    }

    try {
        // 先检查文件是否存在
        const checkResponse = await fetch('/api/downloads');
        if (checkResponse.ok) {
            const data = await checkResponse.json();
            const fileExists = data.files.some(f => f.name === filename);
            
            if (!fileExists) {
                showToast('该客户端暂未提供，请稍后再试', 'error');
                return;
            }
        }
        
        showToast('正在准备下载...', 'info');

        // 直接使用 window.location.href 下载（更可靠）
        window.location.href = `/api/download/${filename}`;
        
        setTimeout(() => {
            showToast('下载已开始', 'success');
        }, 500);
        
    } catch (error) {
        console.error('Download error:', error);
        // 降级处理：直接跳转到下载目录
        window.location.href = `/downloads/${filename}`;
    }
}
