#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QCIRCULARBAR_LIB)
#  define QCIRCULARBAR_EXPORT Q_DECL_EXPORT
# else
#  define QCIRCULARBAR_EXPORT Q_DECL_IMPORT
# endif
#else
# define QCIRCULARBAR_EXPORT
#endif
