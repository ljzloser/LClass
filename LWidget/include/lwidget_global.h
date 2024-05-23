#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LWIDGET_LIB)
#  define LWIDGET_EXPORT Q_DECL_EXPORT
# else
#  define LWIDGET_EXPORT Q_DECL_IMPORT
# endif
#else
# define LWIDGET_EXPORT
#endif
