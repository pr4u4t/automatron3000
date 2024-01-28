#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QKONSOLE_LIB)
#  define QKONSOLE_EXPORT Q_DECL_EXPORT
# else
#  define QKONSOLE_EXPORT Q_DECL_IMPORT
# endif
#else
# define QKONSOLE_EXPORT
#endif
