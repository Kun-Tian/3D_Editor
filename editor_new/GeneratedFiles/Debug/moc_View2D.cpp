/****************************************************************************
** Meta object code from reading C++ file 'View2D.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../View2D.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'View2D.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_GraphicsView_t {
    QByteArrayData data[1];
    char stringdata[13];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GraphicsView_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GraphicsView_t qt_meta_stringdata_GraphicsView = {
    {
QT_MOC_LITERAL(0, 0, 12) // "GraphicsView"

    },
    "GraphicsView"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GraphicsView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void GraphicsView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject GraphicsView::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_GraphicsView.data,
      qt_meta_data_GraphicsView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *GraphicsView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GraphicsView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_GraphicsView.stringdata))
        return static_cast<void*>(const_cast< GraphicsView*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int GraphicsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_View2D_t {
    QByteArrayData data[15];
    char stringdata[165];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_View2D_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_View2D_t qt_meta_stringdata_View2D = {
    {
QT_MOC_LITERAL(0, 0, 6), // "View2D"
QT_MOC_LITERAL(1, 7, 6), // "zoomIn"
QT_MOC_LITERAL(2, 14, 0), // ""
QT_MOC_LITERAL(3, 15, 5), // "level"
QT_MOC_LITERAL(4, 21, 7), // "zoomOut"
QT_MOC_LITERAL(5, 29, 9), // "resetView"
QT_MOC_LITERAL(6, 39, 21), // "setResetButtonEnabled"
QT_MOC_LITERAL(7, 61, 11), // "setupMatrix"
QT_MOC_LITERAL(8, 73, 17), // "togglePointerMode"
QT_MOC_LITERAL(9, 91, 12), // "toggleOpenGL"
QT_MOC_LITERAL(10, 104, 18), // "toggleAntialiasing"
QT_MOC_LITERAL(11, 123, 5), // "print"
QT_MOC_LITERAL(12, 129, 10), // "rotateLeft"
QT_MOC_LITERAL(13, 140, 11), // "rotateRight"
QT_MOC_LITERAL(14, 152, 12) // "deleteVertex"

    },
    "View2D\0zoomIn\0\0level\0zoomOut\0resetView\0"
    "setResetButtonEnabled\0setupMatrix\0"
    "togglePointerMode\0toggleOpenGL\0"
    "toggleAntialiasing\0print\0rotateLeft\0"
    "rotateRight\0deleteVertex"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_View2D[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x0a /* Public */,
       1,    0,   87,    2, 0x2a /* Public | MethodCloned */,
       4,    1,   88,    2, 0x0a /* Public */,
       4,    0,   91,    2, 0x2a /* Public | MethodCloned */,
       5,    0,   92,    2, 0x08 /* Private */,
       6,    0,   93,    2, 0x08 /* Private */,
       7,    0,   94,    2, 0x08 /* Private */,
       8,    0,   95,    2, 0x08 /* Private */,
       9,    0,   96,    2, 0x08 /* Private */,
      10,    0,   97,    2, 0x08 /* Private */,
      11,    0,   98,    2, 0x08 /* Private */,
      12,    0,   99,    2, 0x08 /* Private */,
      13,    0,  100,    2, 0x08 /* Private */,
      14,    0,  101,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void View2D::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        View2D *_t = static_cast<View2D *>(_o);
        switch (_id) {
        case 0: _t->zoomIn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->zoomIn(); break;
        case 2: _t->zoomOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->zoomOut(); break;
        case 4: _t->resetView(); break;
        case 5: _t->setResetButtonEnabled(); break;
        case 6: _t->setupMatrix(); break;
        case 7: _t->togglePointerMode(); break;
        case 8: _t->toggleOpenGL(); break;
        case 9: _t->toggleAntialiasing(); break;
        case 10: _t->print(); break;
        case 11: _t->rotateLeft(); break;
        case 12: _t->rotateRight(); break;
        case 13: _t->deleteVertex(); break;
        default: ;
        }
    }
}

const QMetaObject View2D::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_View2D.data,
      qt_meta_data_View2D,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *View2D::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *View2D::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_View2D.stringdata))
        return static_cast<void*>(const_cast< View2D*>(this));
    return QFrame::qt_metacast(_clname);
}

int View2D::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
