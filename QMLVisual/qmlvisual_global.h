#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QMLVISUAL_LIB)
#  define QMLVISUAL_EXPORT Q_DECL_EXPORT
# else
#  define QMLVISUAL_EXPORT Q_DECL_IMPORT
# endif
#else
# define QMLVISUAL_EXPORT
#endif
