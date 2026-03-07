#include "imagecache.h"
#include "../core/logger.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

ImageCache* ImageCache::instance()
{
    static ImageCache instance;
    return &instance;
}

ImageCache::ImageCache(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_maxDiskCacheSize(DEFAULT_DISK_CACHE_SIZE)
{
    // 设置内存缓存大小（KB）
    m_memoryCache.setMaxCost(DEFAULT_MEMORY_CACHE_SIZE);
    
    // 设置磁盘缓存目录
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/images";
    QDir().mkpath(cacheDir);
    m_diskCacheDir = cacheDir;
    
    Logger::instance()->info(QString("ImageCache initialized, disk cache: %1").arg(cacheDir), "cache");
    
    connect(m_networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            onDownloadFinished();
        } else {
            onDownloadFailed();
        }
    });
}

ImageCache::~ImageCache()
{
}

void ImageCache::loadImage(const QString &url)
{
    if (url.isEmpty()) {
        emit imageLoadFailed(url, "Empty URL");
        return;
    }
    
    // 检查内存缓存
    QPixmap *cached = m_memoryCache.object(url);
    if (cached) {
        Logger::instance()->debug(QString("Image cache hit (memory): %1").arg(url), "cache");
        emit imageLoaded(url, *cached);
        return;
    }
    
    // 检查磁盘缓存
    QPixmap diskCached = loadFromDisk(url);
    if (!diskCached.isNull()) {
        Logger::instance()->debug(QString("Image cache hit (disk): %1").arg(url), "cache");
        // 添加到内存缓存
        m_memoryCache.insert(url, new QPixmap(diskCached));
        emit imageLoaded(url, diskCached);
        return;
    }
    
    // 检查是否已在下载中
    for (auto it = m_pendingRequests.begin(); it != m_pendingRequests.end(); ++it) {
        if (it.value() == url) {
            Logger::instance()->debug(QString("Image already downloading: %1").arg(url), "cache");
            return;
        }
    }
    
    // 下载图片
    Logger::instance()->debug(QString("Downloading image: %1").arg(url), "cache");
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    QNetworkReply *reply = m_networkManager->get(request);
    m_pendingRequests.insert(reply, url);
}

QPixmap ImageCache::getCachedImage(const QString &url) const
{
    QPixmap *cached = m_memoryCache.object(url);
    if (cached) {
        return *cached;
    }
    
    // 尝试从磁盘加载（const 方法中不插入内存缓存）
    QString fileName = urlToFileName(url);
    QString filePath = m_diskCacheDir + "/" + fileName;
    if (QFile::exists(filePath)) {
        QPixmap pixmap(filePath);
        return pixmap;
    }
    
    return QPixmap();
}

bool ImageCache::isCached(const QString &url) const
{
    if (m_memoryCache.object(url)) {
        return true;
    }
    
    QString fileName = urlToFileName(url);
    QString filePath = m_diskCacheDir + "/" + fileName;
    return QFile::exists(filePath);
}

void ImageCache::clear()
{
    clearMemory();
    clearDisk();
    emit cacheCleared();
}

void ImageCache::clearMemory()
{
    QMutexLocker locker(&m_mutex);
    m_memoryCache.clear();
    Logger::instance()->debug("Memory cache cleared", "cache");
}

void ImageCache::clearDisk()
{
    QMutexLocker locker(&m_mutex);
    QDir dir(m_diskCacheDir);
    dir.removeRecursively();
    QDir().mkpath(m_diskCacheDir);
    Logger::instance()->debug("Disk cache cleared", "cache");
}

void ImageCache::remove(const QString &url)
{
    QMutexLocker locker(&m_mutex);
    m_memoryCache.remove(url);
    
    QString fileName = urlToFileName(url);
    QString filePath = m_diskCacheDir + "/" + fileName;
    if (QFile::exists(filePath)) {
        QFile::remove(filePath);
    }
}

void ImageCache::onDownloadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    QString url = m_pendingRequests.take(reply);
    reply->deleteLater();
    
    QByteArray data = reply->readAll();
    QPixmap pixmap;
    if (pixmap.loadFromData(data)) {
        // 保存到内存缓存
        m_memoryCache.insert(url, new QPixmap(pixmap));
        
        // 保存到磁盘缓存
        saveToDisk(url, pixmap);
        
        Logger::instance()->debug(QString("Image downloaded and cached: %1").arg(url), "cache");
        emit imageLoaded(url, pixmap);
    } else {
        Logger::instance()->warning(QString("Failed to parse image: %1").arg(url), "cache");
        emit imageLoadFailed(url, "Invalid image data");
    }
}

void ImageCache::onDownloadFailed()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    QString url = m_pendingRequests.take(reply);
    QString error = reply->errorString();
    reply->deleteLater();
    
    Logger::instance()->warning(QString("Failed to download image %1: %2").arg(url).arg(error), "cache");
    emit imageLoadFailed(url, error);
}

QString ImageCache::getDiskCachePath(const QString &url) const
{
    return m_diskCacheDir + "/" + urlToFileName(url);
}

QString ImageCache::urlToFileName(const QString &url) const
{
    // 使用 MD5 哈希作为文件名，避免 URL 中的特殊字符
    QByteArray hash = QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5);
    QString fileName = QString::fromLatin1(hash.toHex());
    
    // 添加扩展名（从 URL 推断）
    if (url.contains(".png", Qt::CaseInsensitive)) {
        fileName += ".png";
    } else if (url.contains(".jpg", Qt::CaseInsensitive) || url.contains(".jpeg", Qt::CaseInsensitive)) {
        fileName += ".jpg";
    } else if (url.contains(".gif", Qt::CaseInsensitive)) {
        fileName += ".gif";
    } else if (url.contains(".webp", Qt::CaseInsensitive)) {
        fileName += ".webp";
    } else {
        fileName += ".dat";
    }
    
    return fileName;
}

void ImageCache::saveToDisk(const QString &url, const QPixmap &pixmap)
{
    QString filePath = getDiskCachePath(url);
    
    // 检查磁盘空间
    if (diskCacheSize() > m_maxDiskCacheSize) {
        cleanupDiskCache();
    }
    
    // 保存图片
    if (pixmap.save(filePath)) {
        Logger::instance()->debug(QString("Image saved to disk: %1").arg(filePath), "cache");
    }
}

QPixmap ImageCache::loadFromDisk(const QString &url)
{
    QString filePath = getDiskCachePath(url);
    if (QFile::exists(filePath)) {
        QPixmap pixmap(filePath);
        if (!pixmap.isNull()) {
            return pixmap;
        }
    }
    return QPixmap();
}

int ImageCache::diskCacheSize() const
{
    int totalSize = 0;
    QDir dir(m_diskCacheDir);
    QFileInfoList files = dir.entryInfoList(QDir::Files);
    for (const QFileInfo &file : files) {
        totalSize += file.size();
    }
    return totalSize;
}

void ImageCache::cleanupDiskCache()
{
    // 简单的清理策略：删除最旧的文件直到总大小低于限制
    QDir dir(m_diskCacheDir);
    QFileInfoList files = dir.entryInfoList(QDir::Files, QDir::Time);  // 按时间排序
    
    while (diskCacheSize() > m_maxDiskCacheSize * 0.8 && !files.isEmpty()) {
        QFileInfo file = files.takeLast();  // 最旧的文件
        QFile::remove(file.filePath());
        Logger::instance()->debug(QString("Cleaned up old cache file: %1").arg(file.filePath()), "cache");
    }
}
