#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QLINWRITEBYID_LIB)
#  define QLINWRITEBYID_EXPORT Q_DECL_EXPORT
# else
#  define QLINWRITEBYID_EXPORT Q_DECL_IMPORT
# endif
#else
# define QLINWRITEBYID_EXPORT
#endif
