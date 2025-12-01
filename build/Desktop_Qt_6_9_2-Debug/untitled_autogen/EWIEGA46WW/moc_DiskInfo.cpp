/****************************************************************************
** Meta object code from reading C++ file 'DiskInfo.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../DiskInfo.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DiskInfo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN8DiskInfoE_t {};
} // unnamed namespace

template <> constexpr inline auto DiskInfo::qt_create_metaobjectdata<qt_meta_tag_ZN8DiskInfoE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DiskInfo",
        "updateReads",
        "",
        "readIOPS",
        "updateWrites",
        "writeIOPS",
        "updateReadThroughput",
        "readThroughput",
        "updateWriteThroughput",
        "writeThroughput",
        "updateDiskInfo"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'updateReads'
        QtMocHelpers::SignalData<void(long)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Long, 3 },
        }}),
        // Signal 'updateWrites'
        QtMocHelpers::SignalData<void(long)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Long, 5 },
        }}),
        // Signal 'updateReadThroughput'
        QtMocHelpers::SignalData<void(double)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 7 },
        }}),
        // Signal 'updateWriteThroughput'
        QtMocHelpers::SignalData<void(double)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 9 },
        }}),
        // Slot 'updateDiskInfo'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DiskInfo, qt_meta_tag_ZN8DiskInfoE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DiskInfo::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8DiskInfoE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8DiskInfoE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8DiskInfoE_t>.metaTypes,
    nullptr
} };

void DiskInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DiskInfo *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->updateReads((*reinterpret_cast< std::add_pointer_t<long>>(_a[1]))); break;
        case 1: _t->updateWrites((*reinterpret_cast< std::add_pointer_t<long>>(_a[1]))); break;
        case 2: _t->updateReadThroughput((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 3: _t->updateWriteThroughput((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 4: _t->updateDiskInfo(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DiskInfo::*)(long )>(_a, &DiskInfo::updateReads, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DiskInfo::*)(long )>(_a, &DiskInfo::updateWrites, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DiskInfo::*)(double )>(_a, &DiskInfo::updateReadThroughput, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DiskInfo::*)(double )>(_a, &DiskInfo::updateWriteThroughput, 3))
            return;
    }
}

const QMetaObject *DiskInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DiskInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8DiskInfoE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DiskInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void DiskInfo::updateReads(long _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void DiskInfo::updateWrites(long _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void DiskInfo::updateReadThroughput(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void DiskInfo::updateWriteThroughput(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
