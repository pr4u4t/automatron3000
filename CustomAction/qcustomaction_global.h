#ifndef CUSTOMACTION_GLOBAL_H
#define CUSTOMACTION_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QCUSTOMACTION_LIB)
#  define QCUSTOMACTION_EXPORT Q_DECL_EXPORT
# else
#  define QCUSTOMACTION_EXPORT Q_DECL_IMPORT
# endif
#else
# define QCUSTOMACTION_EXPORT
#endif

#endif