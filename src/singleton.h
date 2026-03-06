#ifndef SINGLETON_H
#define SINGLETON_H

#include <QObject>
#include <QMutex>

// 线程安全的单例宏（Qt 5.14.2推荐写法）
#define SINGLETON_DECL(Class) \
    public: \
        static Class* instance(); \
        static void destroy(); \
    private: \
        static Class* s_instance; \
        static QMutex s_mutex; \
        Class() = default; \
        ~Class() = default; \
        Q_DISABLE_COPY(Class)

#define SINGLETON_IMPL(Class) \
    Class* Class::s_instance = nullptr; \
    QMutex Class::s_mutex; \
    Class* Class::instance() { \
        if (!s_instance) { \
            QMutexLocker locker(&s_mutex); \
            if (!s_instance) s_instance = new Class(); \
        } \
        return s_instance; \
    } \
    void Class::destroy() { \
        QMutexLocker locker(&s_mutex); \
        delete s_instance; \
        s_instance = nullptr; \
    }

#endif // SINGLETON_H
