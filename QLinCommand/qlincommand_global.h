#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QLINCOMMAND_LIB)
#  define QLINCOMMAND_EXPORT Q_DECL_EXPORT
# else
#  define QLINCOMMAND_EXPORT Q_DECL_IMPORT
# endif
#else
# define QLINCOMMAND_EXPORT
#endif
