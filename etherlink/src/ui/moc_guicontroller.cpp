/****************************************************************************
** Meta object code from reading C++ file 'guicontroller.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "guicontroller.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guicontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      34,   15,   14,   14, 0x08,
      63,   14,   14,   14, 0x08,
      89,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GuiController[] = {
    "GuiController\0\0packet,num,portIdx\0"
    "streamPacket(uint*,uint,int)\0"
    "readPacketNumber(QString)\0startStream()\0"
};

void GuiController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GuiController *_t = static_cast<GuiController *>(_o);
        switch (_id) {
        case 0: _t->streamPacket((*reinterpret_cast< uint*(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->readPacketNumber((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->startStream(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GuiController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GuiController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GuiController,
      qt_meta_data_GuiController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiController))
        return static_cast<void*>(const_cast< GuiController*>(this));
    return QObject::qt_metacast(_clname);
}

int GuiController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
