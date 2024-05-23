#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LCORE_LIB)
#  define LCORE_EXPORT Q_DECL_EXPORT
# else
#  define LCORE_EXPORT Q_DECL_IMPORT
# endif
#else
# define LCORE_EXPORT
#endif
