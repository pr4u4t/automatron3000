#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(API_LIB)
#  define API_EXPORT Q_DECL_EXPORT
# else
#  define API_EXPORT Q_DECL_IMPORT
# endif
#else
# define API_EXPORT
#endif
