#include "servicelocator.h"
#include "../core/logger.h"

ServiceLocator* ServiceLocator::instance()
{
    static ServiceLocator instance;
    return &instance;
}

ServiceLocator::ServiceLocator(QObject *parent)
    : QObject(parent)
{
    Logger::instance()->debug("ServiceLocator initialized", "di");
}

ServiceLocator::~ServiceLocator()
{
    QMutexLocker locker(&m_mutex);
    
    // 清理拥有的服务
    for (auto it = m_services.begin(); it != m_services.end(); ++it) {
        if (m_ownedServices.value(it.key(), false) && it.value()) {
            delete it.value();
        }
    }
    
    m_services.clear();
    m_ownedServices.clear();
}

bool ServiceLocator::hasService(const QString &name) const
{
    QMutexLocker locker(&m_mutex);
    return m_services.contains(name);
}

QStringList ServiceLocator::registeredServices() const
{
    QMutexLocker locker(&m_mutex);
    return m_services.keys();
}

void ServiceLocator::removeService(const QString &name)
{
    QMutexLocker locker(&m_mutex);
    
    auto it = m_services.find(name);
    if (it != m_services.end()) {
        if (m_ownedServices.value(name, false) && it.value()) {
            delete it.value();
        }
        m_services.erase(it);
        m_ownedServices.remove(name);
        emit serviceRemoved(name);
        Logger::instance()->debug(QString("Service removed: %1").arg(name), "di");
    }
}

void ServiceLocator::clear()
{
    QMutexLocker locker(&m_mutex);
    
    for (auto it = m_services.begin(); it != m_services.end(); ++it) {
        if (m_ownedServices.value(it.key(), false) && it.value()) {
            delete it.value();
        }
    }
    
    m_services.clear();
    m_ownedServices.clear();
    Logger::instance()->debug("All services cleared", "di");
}
