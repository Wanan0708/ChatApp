#ifndef SERVICELOCATOR_H
#define SERVICELOCATOR_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QVariant>
#include <QMutex>

/**
 * @brief 服务定位器（依赖注入容器）
 * 
 * 提供统一的依赖注入和管理功能
 * - 服务注册
 * - 服务解析
 * - 生命周期管理
 * - 线程安全
 */
class ServiceLocator : public QObject
{
    Q_OBJECT
public:
    static ServiceLocator* instance();

    // 服务注册
    template<typename T>
    void registerService(const QString &name, T *instance, bool owned = true) {
        QMutexLocker locker(&m_mutex);
        m_services[name] = instance;
        m_ownedServices[name] = owned;
    }

    // 服务解析
    template<typename T>
    T* resolve(const QString &name) {
        QMutexLocker locker(&m_mutex);
        auto it = m_services.find(name);
        if (it != m_services.end()) {
            return qobject_cast<T*>(*it);
        }
        return nullptr;
    }

    // 检查服务是否已注册
    Q_INVOKABLE bool hasService(const QString &name) const;
    
    // 获取所有已注册服务名称
    Q_INVOKABLE QStringList registeredServices() const;
    
    // 移除服务
    Q_INVOKABLE void removeService(const QString &name);
    
    // 清空所有服务
    Q_INVOKABLE void clear();

signals:
    void serviceRegistered(const QString &name);
    void serviceRemoved(const QString &name);

private:
    explicit ServiceLocator(QObject *parent = nullptr);
    ~ServiceLocator() override;

    mutable QMutex m_mutex;
    QHash<QString, QObject*> m_services;
    QHash<QString, bool> m_ownedServices;  // 是否由 ServiceLocator 拥有
};

// 便捷宏定义
#define REGISTER_SERVICE(name, instance) \
    ServiceLocator::instance()->registerService(name, instance, true)

#define RESOLVE_SERVICE(type, name) \
    ServiceLocator::instance()->resolve<type>(name)

#define HAS_SERVICE(name) \
    ServiceLocator::instance()->hasService(name)

#endif // SERVICELOCATOR_H
