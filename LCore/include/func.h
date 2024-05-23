#pragma once
#include <QString>
#include "lcore_global.h"
namespace ljz
{
	class LCORE_EXPORT LFunc
	{
	public:
		static QString escapeSpecialCharacters(const QString& text);
	};
}
