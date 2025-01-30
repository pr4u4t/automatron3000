#ifndef CHART_GLOBAL_H
#define CHART_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CHART_LIB)
#  define CHART_EXPORT Q_DECL_EXPORT
# else
#  define CHART_EXPORT Q_DECL_IMPORT
# endif
#else
# define CHART_EXPORT
#endif

#endif