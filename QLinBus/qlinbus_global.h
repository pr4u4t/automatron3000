#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QLINBUS_LIB)
#  define QLINBUS_EXPORT Q_DECL_EXPORT
# else
#  define QLINBUS_EXPORT Q_DECL_IMPORT
# endif
#else
# define QLINBUS_EXPORT
#endif
