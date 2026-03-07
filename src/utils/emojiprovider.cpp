#include "emojiprovider.h"
#include "../core/logger.h"
#include <QRegularExpression>

EmojiProvider* EmojiProvider::instance()
{
    static EmojiProvider instance;
    return &instance;
}

EmojiProvider::EmojiProvider(QObject *parent)
    : QObject(parent)
{
    initializeEmojis();
    Logger::instance()->debug("EmojiProvider initialized", "emoji");
}

EmojiProvider::~EmojiProvider()
{
}

void EmojiProvider::initializeEmojis()
{
    // 笑脸表情
    m_categories << "smileys" << "people" << "animals" << "food" << "activities" << "objects" << "symbols" << "flags";
    
    // 常用表情（简化版，实际应用可扩展）
    QList<Emoji> smileys = {
        {"grinning", "😀", "笑脸", "smileys", "笑，开心"},
        {"smiley", "😃", "大笑", "smileys", "笑，开心"},
        {"smile", "😄", "微笑", "smileys", "笑，眼睛"},
        {"grin", "😁", "露齿笑", "smileys", "笑，牙齿"},
        {"laughing", "😆", "大笑", "smileys", "笑，眯眼"},
        {"joy", "😂", "笑哭", "smileys", "笑，泪"},
        {"rofl", "🤣", "滚动笑", "smileys", "笑，地板"},
        {"relaxed", "☺️", "微笑", "smileys", "笑，放松"},
        {"blush", "😊", "脸红", "smileys", "笑，害羞"},
        {"innocent", "😇", "天使", "smileys", "天使，光环"},
        {"slight_smile", "🙂", "微徽", "smileys", "微笑"},
        {"upside_down", "🙃", "倒立", "smileys", "倒立，脸"},
        {"wink", "😉", "眨眼", "smileys", "眨眼，调情"},
        {"relieved", "😌", "释然", "smileys", "放松，安慰"},
        {"heart_eyes", "😍", "爱心眼", "smileys", "爱，眼睛，心"},
        {"kissing_heart", "😘", "飞吻", "smileys", "吻，心"},
        {"kissing", "😗", "亲吻", "smileys", "吻"},
        {"kissing_smiling_eyes", "😙", "微笑吻", "smileys", "吻，眼睛"},
        {"kissing_closed_eyes", "😚", "闭眼吻", "smileys", "吻，闭眼"},
        {"yum", "😋", "美味", "smileys", "吐舌，好吃"},
        {"stuck_out_tongue", "😛", "吐舌", "smileys", "吐舌"},
        {"stuck_out_tongue_winking_eye", "😜", "眨眼吐舌", "smileys", "眨眼，吐舌"},
        {"stuck_out_tongue_closed_eyes", "😝", "闭眼吐舌", "smileys", "闭眼，吐舌"},
        {"money_mouth", "🤑", "钱嘴", "smileys", "钱，嘴"},
        {"hugs", "🤗", "拥抱", "smileys", "拥抱"},
        {"nerd", "🤓", "书呆子", "smileys", "书呆子，眼镜"},
        {"sunglasses", "😎", "墨镜", "smileys", "太阳镜，酷"},
        {"clown", "🤡", "小丑", "smileys", "小丑"},
        {"cowboy", "🤠", "牛仔", "smileys", "牛仔，帽"},
        {"thinking", "🤔", "思考", "smileys", "思考"},
        {"shushing", "🤫", "嘘", "smileys", "嘘，安静"},
        {"zipper_mouth", "🤐", "拉链嘴", "smileys", "拉链，嘴"},
        {"raised_eyebrow", "🤨", "挑眉", "smileys", "挑眉"},
        {"neutral_face", "😐", "中性脸", "smileys", "中性，无表情"},
        {"expressionless", "😑", "无表情", "smileys", "无表情"},
        {"no_mouth", "😶", "无嘴", "smileys", "无嘴"},
        {"smirk", "😏", "得意笑", "smileys", "得意，笑"},
        {"unamused", "😒", "不高兴", "smileys", "不高兴"},
        {"roll_eyes", "🙄", "翻白眼", "smileys", "翻白眼"},
        {"grimacing", "😬", "鬼脸", "smileys", "鬼脸"},
        {"lying_face", "🤥", "说谎", "smileys", "说谎，鼻子"},
        {"pensive", "😔", "沉思", "smileys", "沉思，伤心"},
        {"sleepy", "😪", "困", "smileys", "困，睡"},
        {"drooling", "🤤", "流口水", "smileys", "流口水"},
        {"sleeping", "😴", "睡觉", "smileys", "睡觉"},
        {"mask", "😷", "口罩", "smileys", "口罩，病"},
        {"face_with_thermometer", "🤒", "温度计", "smileys", "温度计，病"},
        {"face_with_head_bandage", "🤕", "绷带", "smileys", "绷带，伤"},
        {"nauseated", "🤢", "恶心", "smileys", "恶心，吐"},
        {"sneezing", "🤧", "打喷嚏", "smileys", "打喷嚏"},
        {"dizzy_face", "😵", "晕", "smileys", "晕，X 眼"},
        {"exploding_head", "🤯", "爆炸头", "smileys", "爆炸，头"},
        {"cowboy_hat", "🤠", "牛仔帽", "smileys", "牛仔，帽"},
        {"party", "🥳", "派对", "smileys", "派对，帽"},
        {"disguised", "🥸", "伪装", "smileys", "伪装，眼镜"},
        {"sunglasses_cool", "😎", "酷", "smileys", "酷，太阳镜"}
    };
    
    // 添加手势表情
    QList<Emoji> people = {
        {"wave", "👋", "挥手", "people", "挥手，再见"},
        {"raised_back_of_hand", "🤚", "手背", "people", "手背"},
        {"raised_hand", "✋", "举手", "people", "举手，停止"},
        {"vulcan_salute", "🖖", "瓦肯礼", "people", "瓦肯，礼"},
        {"ok_hand", "👌", "OK", "people", "OK，好"},
        {"v", "✌️", "胜利", "people", "胜利，二"},
        {"crossed_fingers", "🤞", "交叉", "people", "交叉，好运"},
        {"love_you", "🤟", "爱你", "people", "爱你，手势"},
        {"metal", "🤘", "金属", "people", "金属，手势"},
        {"call_me", "🤙", "打电话", "people", "打电话，手势"},
        {"point_left", "👈", "指左", "people", "指，左"},
        {"point_right", "👉", "指右", "people", "指，右"},
        {"point_up", "☝️", "指上", "people", "指，上"},
        {"point_down", "👇", "指下", "people", "指，下"},
        {"thumbsup", "👍", "赞", "people", "赞，好"},
        {"thumbsdown", "👎", "踩", "people", "踩，不好"},
        {"fist_raised", "✊", "拳头", "people", "拳头，力量"},
        {"fist_oncoming", "👊", "击拳", "people", "击拳"},
        {"fist_left", "🤛", "左拳", "people", "左拳"},
        {"fist_right", "🤜", "右拳", "people", "右拳"},
        {"clap", "👏", "鼓掌", "people", "鼓掌"},
        {"raised_hands", "🙌", "举手", "people", "举手，庆祝"},
        {"open_hands", "👐", "张开手", "people", "张开，手"},
        {"handshake", "🤝", "握手", "people", "握手"},
        {"pray", "🙏", "祈祷", "people", "祈祷，谢谢"},
        {"writing_hand", "✍️", "写字", "people", "写字，手"},
        {"nail_care", "💅", "美甲", "people", "美甲"},
        {"selfie", "🤳", "自拍", "people", "自拍"},
        {"muscle", "💪", "肌肉", "people", "肌肉，力量"},
        {"brain", "🧠", "大脑", "people", "大脑"},
        {"bone", "🦴", "骨头", "people", "骨头"},
        {"eyes", "👀", "眼睛", "people", "眼睛"},
        {"eye", "👁️", "眼", "people", "眼"},
        {"tongue", "👅", "舌头", "people", "舌头"},
        {"lips", "👄", "嘴唇", "people", "嘴唇"}
    };
    
    // 添加动物表情
    QList<Emoji> animals = {
        {"monkey_face", "🐵", "猴脸", "animals", "猴，脸"},
        {"monkey", "🐒", "猴", "animals", "猴"},
        {"gorilla", "🦍", "大猩猩", "animals", "大猩猩"},
        {"dog", "🐶", "狗脸", "animals", "狗，脸"},
        {"dog2", "🐕", "狗", "animals", "狗"},
        {"guide_dog", "🦮", "导盲犬", "animals", "导盲，犬"},
        {"service_dog", "🐕‍🦺", "服务犬", "animals", "服务，犬"},
        {"poodle", "🐩", "贵宾犬", "animals", "贵宾，犬"},
        {"wolf", "🐺", "狼", "animals", "狼"},
        {"fox", "🦊", "狐狸", "animals", "狐狸"},
        {"raccoon", "🦝", "浣熊", "animals", "浣熊"},
        {"cat", "🐱", "猫脸", "animals", "猫，脸"},
        {"cat2", "🐈", "猫", "animals", "猫"},
        {"black_cat", "🐈‍⬛", "黑猫", "animals", "黑，猫"},
        {"lion", "🦁", "狮子", "animals", "狮子"},
        {"tiger", "🐯", "虎脸", "animals", "虎，脸"},
        {"tiger2", "🐅", "虎", "animals", "虎"},
        {"leopard", "🐆", "豹", "animals", "豹"},
        {"horse", "🐴", "马脸", "animals", "马，脸"},
        {"racehorse", "🐎", "赛马", "animals", "赛，马"},
        {"unicorn", "🦄", "独角兽", "animals", "独角兽"},
        {"zebra", "🦓", "斑马", "animals", "斑马"},
        {"deer", "🦌", "鹿", "animals", "鹿"},
        {"cow", "🐮", "牛脸", "animals", "牛，脸"},
        {"ox", "🐂", "公牛", "animals", "公牛"},
        {"water_buffalo", "🐃", "水牛", "animals", "水，牛"},
        {"cow2", "🐄", "母牛", "animals", "母牛"},
        {"pig", "🐷", "猪脸", "animals", "猪，脸"},
        {"pig2", "🐖", "猪", "animals", "猪"},
        {"boar", "🐗", "野猪", "animals", "野猪"},
        {"pig_nose", "🐽", "猪鼻", "animals", "猪，鼻"},
        {"ram", "🐏", "公羊", "animals", "公羊"},
        {"sheep", "🐑", "羊", "animals", "羊"},
        {"goat", "🐐", "山羊", "animals", "山羊"},
        {"dromedary_camel", "🐪", "骆驼", "animals", "骆驼"},
        {"camel", "🐫", "双峰驼", "animals", "双峰，驼"},
        {"llama", "🦙", "羊驼", "animals", "羊驼"},
        {"giraffe", "🦒", "长颈鹿", "animals", "长颈鹿"},
        {"elephant", "🐘", "大象", "animals", "象"},
        {"rhino", "🦏", "犀牛", "animals", "犀牛"},
        {"hippo", "🦛", "河马", "animals", "河马"},
        {"mouse", "🐭", "鼠脸", "animals", "鼠，脸"},
        {"mouse2", "🐁", "鼠", "animals", "鼠"},
        {"rat", "🐀", "大鼠", "animals", "大鼠"},
        {"hamster", "🐹", "仓鼠", "animals", "仓鼠"},
        {"rabbit", "🐰", "兔脸", "animals", "兔，脸"},
        {"rabbit2", "🐇", "兔", "animals", "兔"},
        {"chipmunk", "🐿️", "花栗鼠", "animals", "花栗，鼠"},
        {"hedgehog", "🦔", "刺猬", "animals", "刺猬"},
        {"bat", "🦇", "蝙蝠", "animals", "蝙蝠"},
        {"bear", "🐻", "熊", "animals", "熊"},
        {"koala", "🐨", "考拉", "animals", "考拉"},
        {"panda", "🐼", "熊猫", "animals", "熊猫"},
        {"sloth", "🦥", "树懒", "animals", "树懒"},
        {"otter", "🦦", "水獭", "animals", "水獭"},
        {"skunk", "🦨", "臭鼬", "animals", "臭鼬"},
        {"kangaroo", "🦘", "袋鼠", "animals", "袋鼠"},
        {"badger", "🦡", "獾", "animals", "獾"}
    };
    
    // 添加到哈希表
    for (const Emoji &emoji : smileys) {
        m_emojis[emoji.code] = emoji;
    }
    for (const Emoji &emoji : people) {
        m_emojis[emoji.code] = emoji;
    }
    for (const Emoji &emoji : animals) {
        m_emojis[emoji.code] = emoji;
    }
}

QVariantList EmojiProvider::getEmojis(const QString &category) const
{
    QVariantList result;
    
    for (auto it = m_emojis.begin(); it != m_emojis.end(); ++it) {
        const Emoji &emoji = it.value();
        if (category.isEmpty() || emoji.category == category) {
            QVariantMap map;
            map["code"] = emoji.code;
            map["emoji"] = emoji.emoji;
            map["name"] = emoji.name;
            map["category"] = emoji.category;
            map["tags"] = emoji.tags;
            result.append(map);
        }
    }
    
    return result;
}

QStringList EmojiProvider::getCategories() const
{
    return m_categories;
}

QVariantList EmojiProvider::searchEmojis(const QString &keyword) const
{
    QVariantList result;
    
    if (keyword.isEmpty()) {
        return result;
    }
    
    QString lowerKeyword = keyword.toLower();
    
    for (auto it = m_emojis.begin(); it != m_emojis.end(); ++it) {
        const Emoji &emoji = it.value();
        if (emoji.name.toLower().contains(lowerKeyword) ||
            emoji.tags.toLower().contains(lowerKeyword) ||
            emoji.code.toLower().contains(lowerKeyword)) {
            QVariantMap map;
            map["code"] = emoji.code;
            map["emoji"] = emoji.emoji;
            map["name"] = emoji.name;
            map["category"] = emoji.category;
            result.append(map);
        }
    }
    
    return result;
}

void EmojiProvider::addRecent(const QString &emojiCode)
{
    // 检查是否已存在
    for (int i = 0; i < m_recentEmojis.size(); ++i) {
        QVariantMap emoji = m_recentEmojis[i].toMap();
        if (emoji["code"] == emojiCode) {
            m_recentEmojis.removeAt(i);
            break;
        }
    }
    
    // 添加到开头
    const Emoji *emoji = findEmoji(emojiCode);
    if (emoji) {
        QVariantMap map;
        map["code"] = emoji->code;
        map["emoji"] = emoji->emoji;
        map["name"] = emoji->name;
        m_recentEmojis.prepend(map);
        
        // 限制数量
        while (m_recentEmojis.size() > MAX_RECENT) {
            m_recentEmojis.removeLast();
        }
        
        emit recentChanged();
    }
}

void EmojiProvider::toggleFavorite(const QString &emojiCode)
{
    // 检查是否已收藏
    for (int i = 0; i < m_favoriteEmojis.size(); ++i) {
        QVariantMap emoji = m_favoriteEmojis[i].toMap();
        if (emoji["code"] == emojiCode) {
            m_favoriteEmojis.removeAt(i);
            emit favoriteChanged();
            return;
        }
    }
    
    // 添加到收藏
    const Emoji *emoji = findEmoji(emojiCode);
    if (emoji) {
        QVariantMap map;
        map["code"] = emoji->code;
        map["emoji"] = emoji->emoji;
        map["name"] = emoji->name;
        m_favoriteEmojis.append(map);
        emit favoriteChanged();
    }
}

bool EmojiProvider::isFavorite(const QString &emojiCode) const
{
    for (const QVariant &variant : m_favoriteEmojis) {
        QVariantMap emoji = variant.toMap();
        if (emoji["code"] == emojiCode) {
            return true;
        }
    }
    return false;
}

QString EmojiProvider::emojiToUnicode(const QString &code) const
{
    const Emoji *emoji = findEmoji(code);
    if (emoji) {
        return emoji->emoji;
    }
    return code;
}

QString EmojiProvider::unicodeToEmoji(const QString &unicode) const
{
    for (auto it = m_emojis.begin(); it != m_emojis.end(); ++it) {
        if (it.value().emoji == unicode) {
            return it.value().code;
        }
    }
    return unicode;
}

QString EmojiProvider::parseEmojis(const QString &text) const
{
    QString result = text;
    
    // 简单实现：将 :code: 格式转换为表情
    QRegularExpression re(":([a-z0-9_]+):");
    QRegularExpressionMatchIterator it = re.globalMatch(result);
    
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString code = match.captured(1);
        const Emoji *emoji = findEmoji(code);
        if (emoji) {
            result.replace(match.captured(0), emoji->emoji);
        }
    }
    
    return result;
}

const Emoji* EmojiProvider::findEmoji(const QString &code) const
{
    auto it = m_emojis.find(code);
    if (it != m_emojis.end()) {
        return &it.value();
    }
    return nullptr;
}
