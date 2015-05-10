#ifndef QUDUNIT_GLOBAL_H
#define QUDUNIT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QUDUNIT_LIBRARY)
#  define QUDUNITSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QUDUNITSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QUDUNIT_GLOBAL_H
