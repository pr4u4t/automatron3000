#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QLIN_LIB)
#  define QLIN_EXPORT Q_DECL_EXPORT
# else
#  define QLIN_EXPORT Q_DECL_IMPORT
# endif
#else
# define QLIN_EXPORT
#endif
