#ifndef QDATA_TABLE_GLOBAL_H
#define QDATA_TABLE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QDATATABLE_LIB)
#  define QDATATABLE_EXPORT Q_DECL_EXPORT
# else
#  define QDATATABLE_EXPORT Q_DECL_IMPORT
# endif
#else
# define QDATATABLE_EXPORT
#endif

#endif