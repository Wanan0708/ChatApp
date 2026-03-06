/****************************************************************************
** Meta object code from reading C++ file 'userdao.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/dao/userdao.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'userdao.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UserDAO_t {
    QByteArrayData data[20];
    char stringdata0[257];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UserDAO_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UserDAO_t qt_meta_stringdata_UserDAO = {
    {
QT_MOC_LITERAL(0, 0, 7), // "UserDAO"
QT_MOC_LITERAL(1, 8, 11), // "loginResult"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 7), // "success"
QT_MOC_LITERAL(4, 29, 6), // "userId"
QT_MOC_LITERAL(5, 36, 5), // "token"
QT_MOC_LITERAL(6, 42, 5), // "error"
QT_MOC_LITERAL(7, 48, 14), // "registerResult"
QT_MOC_LITERAL(8, 63, 14), // "userInfoResult"
QT_MOC_LITERAL(9, 78, 8), // "username"
QT_MOC_LITERAL(10, 87, 17), // "userProfileResult"
QT_MOC_LITERAL(11, 105, 7), // "profile"
QT_MOC_LITERAL(12, 113, 18), // "userProfileUpdated"
QT_MOC_LITERAL(13, 132, 12), // "searchResult"
QT_MOC_LITERAL(14, 145, 25), // "QVector<UserSearchResult>"
QT_MOC_LITERAL(15, 171, 7), // "results"
QT_MOC_LITERAL(16, 179, 26), // "passwordResetCodeGenerated"
QT_MOC_LITERAL(17, 206, 9), // "resetCode"
QT_MOC_LITERAL(18, 216, 19), // "passwordResetResult"
QT_MOC_LITERAL(19, 236, 20) // "passwordUpdateResult"

    },
    "UserDAO\0loginResult\0\0success\0userId\0"
    "token\0error\0registerResult\0userInfoResult\0"
    "username\0userProfileResult\0profile\0"
    "userProfileUpdated\0searchResult\0"
    "QVector<UserSearchResult>\0results\0"
    "passwordResetCodeGenerated\0resetCode\0"
    "passwordResetResult\0passwordUpdateResult"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UserDAO[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      17,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   99,    2, 0x06 /* Public */,
       1,    3,  108,    2, 0x26 /* Public | MethodCloned */,
       7,    3,  115,    2, 0x06 /* Public */,
       7,    2,  122,    2, 0x26 /* Public | MethodCloned */,
       8,    3,  127,    2, 0x06 /* Public */,
       8,    2,  134,    2, 0x26 /* Public | MethodCloned */,
      10,    3,  139,    2, 0x06 /* Public */,
      10,    2,  146,    2, 0x26 /* Public | MethodCloned */,
      12,    2,  151,    2, 0x06 /* Public */,
      12,    1,  156,    2, 0x26 /* Public | MethodCloned */,
      13,    1,  159,    2, 0x06 /* Public */,
      16,    3,  162,    2, 0x06 /* Public */,
      16,    2,  169,    2, 0x26 /* Public | MethodCloned */,
      18,    2,  174,    2, 0x06 /* Public */,
      18,    1,  179,    2, 0x26 /* Public | MethodCloned */,
      19,    2,  182,    2, 0x06 /* Public */,
      19,    1,  187,    2, 0x26 /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString, QMetaType::QString,    3,    4,    5,    6,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString,    3,    4,    5,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString,    3,    4,    6,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString,    3,    9,    6,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    3,    9,
    QMetaType::Void, QMetaType::Bool, QMetaType::QVariantMap, QMetaType::QString,    3,   11,    6,
    QMetaType::Void, QMetaType::Bool, QMetaType::QVariantMap,    3,   11,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    3,    6,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString,    3,   17,    6,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    3,   17,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    3,    6,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    3,    6,
    QMetaType::Void, QMetaType::Bool,    3,

       0        // eod
};

void UserDAO::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UserDAO *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loginResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 1: _t->loginResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 2: _t->registerResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 3: _t->registerResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->userInfoResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 5: _t->userInfoResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->userProfileResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QVariantMap(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 7: _t->userProfileResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QVariantMap(*)>(_a[2]))); break;
        case 8: _t->userProfileUpdated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 9: _t->userProfileUpdated((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->searchResult((*reinterpret_cast< const QVector<UserSearchResult>(*)>(_a[1]))); break;
        case 11: _t->passwordResetCodeGenerated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 12: _t->passwordResetCodeGenerated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 13: _t->passwordResetResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 14: _t->passwordResetResult((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->passwordUpdateResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 16: _t->passwordUpdateResult((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UserDAO::*)(bool , const QString & , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UserDAO::loginResult)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UserDAO::*)(bool , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UserDAO::registerResult)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (UserDAO::*)(bool , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UserDAO::userInfoResult)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (UserDAO::*)(bool , const QVariantMap & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UserDAO::userProfileResult)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (UserDAO::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UserDAO::userProfileUpdated)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (UserDAO::*)(const QVector<UserSearchResult> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UserDAO::searchResult)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (UserDAO::*)(bool , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UserDAO::passwordResetCodeGenerated)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (UserDAO::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UserDAO::passwordResetResult)) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (UserDAO::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UserDAO::passwordUpdateResult)) {
                *result = 15;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UserDAO::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_UserDAO.data,
    qt_meta_data_UserDAO,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UserDAO::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UserDAO::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UserDAO.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UserDAO::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void UserDAO::loginResult(bool _t1, const QString & _t2, const QString & _t3, const QString & _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 2
void UserDAO::registerResult(bool _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 4
void UserDAO::userInfoResult(bool _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 6
void UserDAO::userProfileResult(bool _t1, const QVariantMap & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 8
void UserDAO::userProfileUpdated(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 10
void UserDAO::searchResult(const QVector<UserSearchResult> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void UserDAO::passwordResetCodeGenerated(bool _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 13
void UserDAO::passwordResetResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 15
void UserDAO::passwordUpdateResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
