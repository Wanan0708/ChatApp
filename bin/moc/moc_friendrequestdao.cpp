/****************************************************************************
** Meta object code from reading C++ file 'friendrequestdao.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/dao/friendrequestdao.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'friendrequestdao.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FriendRequestDAO_t {
    QByteArrayData data[14];
    char stringdata0[178];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FriendRequestDAO_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FriendRequestDAO_t qt_meta_stringdata_FriendRequestDAO = {
    {
QT_MOC_LITERAL(0, 0, 16), // "FriendRequestDAO"
QT_MOC_LITERAL(1, 17, 11), // "requestSent"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 7), // "success"
QT_MOC_LITERAL(4, 38, 5), // "error"
QT_MOC_LITERAL(5, 44, 21), // "pendingRequestsLoaded"
QT_MOC_LITERAL(6, 66, 22), // "QVector<FriendRequest>"
QT_MOC_LITERAL(7, 89, 8), // "requests"
QT_MOC_LITERAL(8, 98, 20), // "requestStatusUpdated"
QT_MOC_LITERAL(9, 119, 9), // "requestId"
QT_MOC_LITERAL(10, 129, 6), // "status"
QT_MOC_LITERAL(11, 136, 13), // "friendsLoaded"
QT_MOC_LITERAL(12, 150, 19), // "QVector<FriendInfo>"
QT_MOC_LITERAL(13, 170, 7) // "friends"

    },
    "FriendRequestDAO\0requestSent\0\0success\0"
    "error\0pendingRequestsLoaded\0"
    "QVector<FriendRequest>\0requests\0"
    "requestStatusUpdated\0requestId\0status\0"
    "friendsLoaded\0QVector<FriendInfo>\0"
    "friends"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FriendRequestDAO[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       1,    1,   44,    2, 0x26 /* Public | MethodCloned */,
       5,    1,   47,    2, 0x06 /* Public */,
       8,    3,   50,    2, 0x06 /* Public */,
      11,    1,   57,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int, QMetaType::QString,    3,    9,   10,
    QMetaType::Void, 0x80000000 | 12,   13,

       0        // eod
};

void FriendRequestDAO::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FriendRequestDAO *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestSent((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->requestSent((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->pendingRequestsLoaded((*reinterpret_cast< const QVector<FriendRequest>(*)>(_a[1]))); break;
        case 3: _t->requestStatusUpdated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 4: _t->friendsLoaded((*reinterpret_cast< const QVector<FriendInfo>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FriendRequestDAO::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FriendRequestDAO::requestSent)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FriendRequestDAO::*)(const QVector<FriendRequest> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FriendRequestDAO::pendingRequestsLoaded)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (FriendRequestDAO::*)(bool , int , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FriendRequestDAO::requestStatusUpdated)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (FriendRequestDAO::*)(const QVector<FriendInfo> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FriendRequestDAO::friendsLoaded)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FriendRequestDAO::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_FriendRequestDAO.data,
    qt_meta_data_FriendRequestDAO,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FriendRequestDAO::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FriendRequestDAO::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FriendRequestDAO.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FriendRequestDAO::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void FriendRequestDAO::requestSent(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 2
void FriendRequestDAO::pendingRequestsLoaded(const QVector<FriendRequest> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void FriendRequestDAO::requestStatusUpdated(bool _t1, int _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void FriendRequestDAO::friendsLoaded(const QVector<FriendInfo> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
