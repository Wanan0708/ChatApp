#include "translationmanager.h"
#include "logger.h"

TranslationManager* TranslationManager::instance()
{
    static TranslationManager instance;
    return &instance;
}

TranslationManager::TranslationManager(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings(this))
{
    initializeTranslations();
    load();
    
    Logger::instance()->info(QString("TranslationManager initialized with language: %1").arg(m_currentLanguage), "i18n");
}

TranslationManager::~TranslationManager()
{
    save();
}

void TranslationManager::initializeTranslations()
{
    // 中文（简体）
    QHash<QString, QString> zhCN;
    zhCN["app.name"] = "ESChat";
    zhCN["app.slogan"] = "安全、极速、私密的交流体验";
    zhCN["app.copyright"] = "© 2026 ESChat · 让沟通更简单";
    
    zhCN["login.title"] = "登录";
    zhCN["login.username"] = "请输入用户名";
    zhCN["login.password"] = "请输入密码";
    zhCN["login.remember"] = "记住我的登录信息";
    zhCN["login.button"] = "登 录";
    zhCN["login.register"] = "立即注册";
    zhCN["login.forgotPassword"] = "忘记密码？";
    zhCN["login.noAccount"] = "还没有账户？";
    
    zhCN["register.title"] = "注册";
    zhCN["register.username"] = "用户名";
    zhCN["register.password"] = "密码";
    zhCN["register.confirmPassword"] = "确认密码";
    zhCN["register.button"] = "注 册";
    zhCN["register.back"] = "返回登录";
    
    zhCN["chat.messages"] = "消息";
    zhCN["chat.contacts"] = "联系人";
    zhCN["chat.notifications"] = "通知";
    zhCN["chat.settings"] = "设置";
    zhCN["chat.placeholder"] = "请选择一个会话开始聊天";
    zhCN["chat.inputPlaceholder"] = "输入消息...";
    zhCN["chat.send"] = "发送";
    zhCN["chat.file"] = "发送文件";
    zhCN["chat.image"] = "发送图片";
    zhCN["chat.recalled"] = "消息已撤回";
    
    zhCN["settings.profile"] = "个人资料";
    zhCN["settings.account"] = "账户";
    zhCN["settings.privacy"] = "隐私";
    zhCN["settings.about"] = "关于";
    zhCN["settings.logout"] = "退出登录";
    zhCN["settings.changePassword"] = "修改密码";
    zhCN["settings.theme"] = "主题";
    zhCN["settings.language"] = "语言";
    
    zhCN["common.ok"] = "确定";
    zhCN["common.cancel"] = "取消";
    zhCN["common.yes"] = "是";
    zhCN["common.no"] = "否";
    zhCN["common.loading"] = "加载中...";
    zhCN["common.error"] = "错误";
    zhCN["common.success"] = "成功";
    zhCN["common.retry"] = "重试";
    
    zhCN["error.network"] = "网络连接失败";
    zhCN["error.auth"] = "认证失败";
    zhCN["error.server"] = "服务器错误";
    zhCN["error.timeout"] = "请求超时";
    
    m_languages["zh_CN"] = zhCN;
    
    // 英文
    QHash<QString, QString> enUS;
    enUS["app.name"] = "ESChat";
    enUS["app.slogan"] = "Secure, Fast, Private Messaging";
    enUS["app.copyright"] = "© 2026 ESChat · Making Communication Simpler";
    
    enUS["login.title"] = "Login";
    enUS["login.username"] = "Username";
    enUS["login.password"] = "Password";
    enUS["login.remember"] = "Remember me";
    enUS["login.button"] = "Login";
    enUS["login.register"] = "Register";
    enUS["login.forgotPassword"] = "Forgot Password?";
    enUS["login.noAccount"] = "No account?";
    
    enUS["register.title"] = "Register";
    enUS["register.username"] = "Username";
    enUS["register.password"] = "Password";
    enUS["register.confirmPassword"] = "Confirm Password";
    enUS["register.button"] = "Register";
    enUS["register.back"] = "Back to Login";
    
    enUS["chat.messages"] = "Messages";
    enUS["chat.contacts"] = "Contacts";
    enUS["chat.notifications"] = "Notifications";
    enUS["chat.settings"] = "Settings";
    enUS["chat.placeholder"] = "Select a conversation to start chatting";
    enUS["chat.inputPlaceholder"] = "Type a message...";
    enUS["chat.send"] = "Send";
    enUS["chat.file"] = "Send File";
    enUS["chat.image"] = "Send Image";
    enUS["chat.recalled"] = "Message recalled";
    
    enUS["settings.profile"] = "Profile";
    enUS["settings.account"] = "Account";
    enUS["settings.privacy"] = "Privacy";
    enUS["settings.about"] = "About";
    enUS["settings.logout"] = "Logout";
    enUS["settings.changePassword"] = "Change Password";
    enUS["settings.theme"] = "Theme";
    enUS["settings.language"] = "Language";
    
    enUS["common.ok"] = "OK";
    enUS["common.cancel"] = "Cancel";
    enUS["common.yes"] = "Yes";
    enUS["common.no"] = "No";
    enUS["common.loading"] = "Loading...";
    enUS["common.error"] = "Error";
    enUS["common.success"] = "Success";
    enUS["common.retry"] = "Retry";
    
    enUS["error.network"] = "Network connection failed";
    enUS["error.auth"] = "Authentication failed";
    enUS["error.server"] = "Server error";
    enUS["error.timeout"] = "Request timeout";
    
    m_languages["en_US"] = enUS;
    
    // 默认语言
    m_currentLanguage = "zh_CN";
}

void TranslationManager::setCurrentLanguage(const QString &language)
{
    if (!m_languages.contains(language)) {
        Logger::instance()->warning(QString("Unknown language: %1").arg(language), "i18n");
        return;
    }
    
    if (m_currentLanguage != language) {
        m_currentLanguage = language;
        Logger::instance()->info(QString("Language changed to: %1").arg(language), "i18n");
        emit currentLanguageChanged(language);
        emit translationChanged();
        save();
    }
}

QString TranslationManager::tr(const QString &key) const
{
    auto langIt = m_languages.find(m_currentLanguage);
    if (langIt != m_languages.end()) {
        const auto &translations = *langIt;
        auto it = translations.find(key);
        if (it != translations.end()) {
            return *it;
        }
    }
    
    // 回退到英文
    auto enIt = m_languages.find("en_US");
    if (enIt != m_languages.end()) {
        const auto &translations = *enIt;
        auto it = translations.find(key);
        if (it != translations.end()) {
            return *it;
        }
    }
    
    // 返回 key 本身
    return key;
}

QString TranslationManager::languageName(const QString &code) const
{
    if (code == "zh_CN") return "简体中文";
    if (code == "en_US") return "English";
    return code;
}

void TranslationManager::save()
{
    m_settings->setValue("language/current", m_currentLanguage);
    m_settings->sync();
    Logger::instance()->debug("Language settings saved", "i18n");
}

void TranslationManager::load()
{
    QString savedLang = m_settings->value("language/current", "zh_CN").toString();
    if (m_languages.contains(savedLang)) {
        m_currentLanguage = savedLang;
        Logger::instance()->debug(QString("Loaded language: %1").arg(savedLang), "i18n");
    }
}
