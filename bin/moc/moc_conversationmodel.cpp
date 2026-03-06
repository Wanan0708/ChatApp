/****************************************************************************
** Meta object code from reading C++ file 'conversationmodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/models/conversationmodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'conversationmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ConversationModel_t {
    QByteArrayData data[21];
    char stringdata0[222];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ConversationModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ConversationModel_t qt_meta_stringdata_ConversationModel = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ConversationModel"
QT_MOC_LITERAL(1, 18, 12), // "countChanged"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 16), // "modelDataChanged"
QT_MOC_LITERAL(4, 49, 18), // "updateConversation"
QT_MOC_LITERAL(5, 68, 2), // "id"
QT_MOC_LITERAL(6, 71, 7), // "updates"
QT_MOC_LITERAL(7, 79, 23), // "findOrStartConversation"
QT_MOC_LITERAL(8, 103, 6), // "userId"
QT_MOC_LITERAL(9, 110, 8), // "userName"
QT_MOC_LITERAL(10, 119, 3), // "get"
QT_MOC_LITERAL(11, 123, 5), // "index"
QT_MOC_LITERAL(12, 129, 5), // "count"
QT_MOC_LITERAL(13, 135, 5), // "Roles"
QT_MOC_LITERAL(14, 141, 6), // "IdRole"
QT_MOC_LITERAL(15, 148, 9), // "TitleRole"
QT_MOC_LITERAL(16, 158, 15), // "LastMessageRole"
QT_MOC_LITERAL(17, 174, 8), // "TimeRole"
QT_MOC_LITERAL(18, 183, 15), // "UnreadCountRole"
QT_MOC_LITERAL(19, 199, 8), // "TypeRole"
QT_MOC_LITERAL(20, 208, 13) // "IsCurrentRole"

    },
    "ConversationModel\0countChanged\0\0"
    "modelDataChanged\0updateConversation\0"
    "id\0updates\0findOrStartConversation\0"
    "userId\0userName\0get\0index\0count\0Roles\0"
    "IdRole\0TitleRole\0LastMessageRole\0"
    "TimeRole\0UnreadCountRole\0TypeRole\0"
    "IsCurrentRole"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ConversationModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       1,   54, // properties
       1,   58, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       4,    2,   41,    2, 0x02 /* Public */,
       7,    2,   46,    2, 0x02 /* Public */,
      10,    1,   51,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QVariantMap,    5,    6,
    QMetaType::QString, QMetaType::QString, QMetaType::QString,    8,    9,
    QMetaType::QVariantMap, QMetaType::Int,   11,

 // properties: name, type, flags
      12, QMetaType::Int, 0x00495001,

 // properties: notify_signal_id
       0,

 // enums: name, alias, flags, count, data
      13,   13, 0x0,    7,   63,

 // enum data: key, value
      14, uint(ConversationModel::IdRole),
      15, uint(ConversationModel::TitleRole),
      16, uint(ConversationModel::LastMessageRole),
      17, uint(ConversationModel::TimeRole),
      18, uint(ConversationModel::UnreadCountRole),
      19, uint(ConversationModel::TypeRole),
      20, uint(ConversationModel::IsCurrentRole),

       0        // eod
};

void ConversationModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ConversationModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->countChanged(); break;
        case 1: _t->modelDataChanged(); break;
        case 2: _t->updateConversation((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariantMap(*)>(_a[2]))); break;
        case 3: { QString _r = _t->findOrStartConversation((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 4: { QVariantMap _r = _t->get((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ConversationModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConversationModel::countChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ConversationModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConversationModel::modelDataChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ConversationModel *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->count(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject ConversationModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_meta_stringdata_ConversationModel.data,
    qt_meta_data_ConversationModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ConversationModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConversationModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ConversationModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int ConversationModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ConversationModel::countChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ConversationModel::modelDataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
