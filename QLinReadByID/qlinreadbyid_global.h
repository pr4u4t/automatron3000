#ifndef QLINREADBYID_GLOBAL_H
#define QLINREADBYID_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QLINREADBYID_LIB)
#  define QLINREADBYID_EXPORT Q_DECL_EXPORT
# else
#  define QLINREADBYID_EXPORT Q_DECL_IMPORT
# endif
#else
# define QLINREADBYID_EXPORT
#endif

#endif