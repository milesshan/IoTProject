#pragma once

#ifndef IOT_GLOBAL_H
#define IOT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(IOT_LIB)
#  define IOT_EXPORT Q_DECL_EXPORT
# else
#  define IOT_EXPORT Q_DECL_IMPORT
# endif
#else
# define IOT_EXPORT
#endif

#endif // IOT_GLOBAL_H
