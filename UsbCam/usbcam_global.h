#ifndef USBCAM_GLOBAL_H
#define USBCAM_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(USBCAM_LIB)
#  define USBCAM_EXPORT Q_DECL_EXPORT
# else
#  define USBCAM_EXPORT Q_DECL_IMPORT
# endif
#else
# define USBCAM_EXPORT
#endif

#endif