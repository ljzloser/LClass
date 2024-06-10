#pragma once
#include <QString>
#include <QSettings>
#include "lcore_global.h"
namespace ljz
{
	class LCORE_EXPORT LFunc
	{
	public:
		// 将字符串中的特殊字符转义
		static QString escapeSpecialCharacters(const QString& text);
		// 将字符串中的tab和空格替换为html的标签
		static QString replaceSpaceToHtml(QString& text);
		// 将字符串中的特殊字符转义或反转义
		static QString escapeString(QString text, bool escape);
#ifdef WIN32
		// windows10 以上读取系统是否处于深色模式
		static bool IsSystemDarkModeActive();
#endif
		/**
		 * 开机自启动，windows系统采用注册表方式，linux系统采用配置文件方式
		 * @param isAutoRun 是否开机自启动
		 * @param appName 应用名
		 */
		static void autoRun(int isAutoRun, QString appName);
		/**
		 * @brief 截断字符串并添加省略号
		 * @param str 需要截断的字符串
		 * @param length 截断长度
		 * @return
		 */
		static QString truncateString(const QString& str, int length);
	};
}
