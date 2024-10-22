#ifndef QWEB_GLOBAL_H
#define QWEB_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QWEB_LIB)
#  define QWEB_EXPORT Q_DECL_EXPORT
# else
#  define QWEB_EXPORT Q_DECL_IMPORT
# endif
#else
# define QWEB_EXPORT
#endif

#endif