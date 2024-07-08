#ifndef QLINTESTER_GLOBAL_H
#define QLINTESTER_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QLINTESTER_LIB)
#  define QLINTESTER_EXPORT Q_DECL_EXPORT
# else
#  define QLINTESTER_EXPORT Q_DECL_IMPORT
# endif
#else
# define QLINTESTER_EXPORT
#endif

#endif