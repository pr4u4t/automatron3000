#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QJTAG_LIB)
#  define QJTAG_EXPORT Q_DECL_EXPORT
# else
#  define QJTAG_EXPORT Q_DECL_IMPORT
# endif
#else
# define QJTAG_EXPORT
#endif
