#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <QObject>
#include <QCache>
#include <QPixmap>
#include <QQueue>
#include <QMutex>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @brief 图片缓存管理器
 * 
 * 提供两级图片缓存（内存 + 磁盘）
 * 
 * 特性：
 * - 内存缓存（LRU）
 * - 磁盘缓存
 * - 异步加载
 * - 自动清理
 */
class ImageCache : public QObject
{
    Q_OBJECT
public:
    static ImageCache* instance();

    // 配置
    void setMemoryCacheSize(int bytes) { m_memoryCache.setMaxCost(bytes / 1024); }
    void setDiskCacheSize(int bytes) { m_maxDiskCacheSize = bytes; }
    int memoryCacheSize() const { return m_memoryCache.maxCost() * 1024; }
    int diskCacheSize() const;

    // 图片加载
    Q_INVOKABLE void loadImage(const QString &url);
    Q_INVOKABLE QPixmap getCachedImage(const QString &url) const;
    Q_INVOKABLE bool isCached(const QString &url) const;
    
    // 缓存管理
    Q_INVOKABLE void clear();
    Q_INVOKABLE void clearMemory();
    Q_INVOKABLE void clearDisk();
    Q_INVOKABLE void remove(const QString &url);

signals:
    void imageLoaded(const QString &url, const QPixmap &image);
    void imageLoadFailed(const QString &url, const QString &error);
    void cacheCleared();

private slots:
    void onDownloadFinished();
    void onDownloadFailed();

private:
    explicit ImageCache(QObject *parent = nullptr);
    ~ImageCache() override;

    QString getDiskCachePath(const QString &url) const;
    QString urlToFileName(const QString &url) const;
    void saveToDisk(const QString &url, const QPixmap &pixmap);
    QPixmap loadFromDisk(const QString &url);
    void cleanupDiskCache();

    QCache<QString, QPixmap> m_memoryCache;
    QNetworkAccessManager *m_networkManager;
    QHash<QNetworkReply*, QString> m_pendingRequests;
    QMutex m_mutex;
    
    QString m_diskCacheDir;
    int m_maxDiskCacheSize;
    static const int DEFAULT_MEMORY_CACHE_SIZE = 50 * 1024;  // 50MB
    static const int DEFAULT_DISK_CACHE_SIZE = 200 * 1024 * 1024;  // 200MB
};

#endif // IMAGECACHE_H
