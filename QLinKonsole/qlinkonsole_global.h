#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QLINKONSOLE_LIB)
#  define QLINKONSOLE_EXPORT Q_DECL_EXPORT
# else
#  define QLINKONSOLE_EXPORT Q_DECL_IMPORT
# endif
#else
# define QLINKONSOLE_EXPORT
#endif
