#pragma once
#include <QString>
#include <QSettings>
#include "lcore_global.h"
namespace ljz
{
	class LCORE_EXPORT LFunc
	{
	public:
		static QString escapeSpecialCharacters(const QString& text);
		static QString replaceSpaceToHtml(QString& text);
		static QString escapeString(QString text, bool escape);
		static bool IsSystemDarkModeActive();
		static void autoRun(int isAutoRun, QString appName);
	};
}
