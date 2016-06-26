/****************************************************************************
** Meta object code from reading C++ file 'EthernetTransmit.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "EthernetTransmit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EthernetTransmit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EthernetTransmit[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      32,   18,   17,   17, 0x05,
      56,   17,   17,   17, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_EthernetTransmit[] = {
    "EthernetTransmit\0\0txPacket,size\0"
    "newTxPacket(uint*,uint)\0daignosticsComplete()\0"
};

void EthernetTransmit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EthernetTransmit *_t = static_cast<EthernetTransmit *>(_o);
        switch (_id) {
        case 0: _t->newTxPacket((*reinterpret_cast< uint*(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 1: _t->daignosticsComplete(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EthernetTransmit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EthernetTransmit::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_EthernetTransmit,
      qt_meta_data_EthernetTransmit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EthernetTransmit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EthernetTransmit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EthernetTransmit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EthernetTransmit))
        return static_cast<void*>(const_cast< EthernetTransmit*>(this));
    return QThread::qt_metacast(_clname);
}

int EthernetTransmit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void EthernetTransmit::newTxPacket(unsigned int * _t1, unsigned int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EthernetTransmit::daignosticsComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
