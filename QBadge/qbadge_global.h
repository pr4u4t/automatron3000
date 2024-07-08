#ifndef QBADGE_GLOBAL_H
#define QBADGE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QBADGE_LIB)
#  define QBADGE_EXPORT Q_DECL_EXPORT
# else
#  define QBADGE_EXPORT Q_DECL_IMPORT
# endif
#else
# define QBADGE_EXPORT
#endif

#endif
