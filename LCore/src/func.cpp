#include "func.h"
#include <QCoreApplication>
#ifdef WIN32
#include <windows.h>
#endif
#include <QDir>
#include <QColor>
using namespace ljz;

QString LFunc::escapeSpecialCharacters(const QString& text)
{
	// 定义所有需要转义的特殊字符
	const QString specialCharacters = R"([\^$.|?*+(){}[\]])";

	// 在每个特殊字符前添加反斜杠进行转义
	QString escapedText;
	for (const QChar& ch : text) {
		if (specialCharacters.contains(ch)) {
			escapedText.append('\\');
		}
		escapedText.append(ch);
	}

	return escapedText;
}

QString LFunc::replaceSpaceToHtml(QString& text)
{
	QString result = text.replace("\t", "&nbsp;&nbsp;&nbsp;&nbsp;");
	return result.replace(" ", "&nbsp;");
}
QString LFunc::escapeString(QString text, bool escape) {
	// 如果为真对文本进行转义
	if (escape)
	{
		// 转义
		text = text.replace("\\", "\\\\");
		text = text.replace("\"", "\\\"");
		text = text.replace("\'", "\\\'");
		text = text.replace("\n", "\\n");
		text = text.replace("\r", "\\r");
		text = text.replace("\t", "\\t");
	}
	else
	{
		//反转义
		text = text.replace("\\\\", "\\");
		text = text.replace("\\\"", "\"");
		text = text.replace("\\\'", "\'");
		text = text.replace("\\n", "\n");
		text = text.replace("\\r", "\r");
		text = text.replace("\\t", "\t");
	}
	return text;
}
#ifdef WIN32
bool LFunc::IsSystemDarkModeActive()
{
	QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
	QVariant themeValue = settings.value("AppsUseLightTheme", QVariant(1)); // 默认使用浅色主题
	return themeValue.isValid() && themeValue.toInt() == 0;
}
QColor LFunc::getSystemAccentColor() {
	DWORD colorValue;
	DWORD colorSize = sizeof(colorValue);
	LONG result = RegGetValue(HKEY_CURRENT_USER,
		L"SOFTWARE\\Microsoft\\Windows\\DWM",
		L"AccentColor",
		RRF_RT_REG_DWORD,
		nullptr,
		&colorValue,
		&colorSize);
	if (result == ERROR_SUCCESS) {
		// Windows stores colors as ABGR
		int red = (colorValue >> 0) & 0xFF;
		int green = (colorValue >> 8) & 0xFF;
		int blue = (colorValue >> 16) & 0xFF;
		return QColor(red, green, blue);
	}
	else {
		qWarning() << "Failed to get accent color from registry";
		return QColor(); // Return default color if reading fails
	}
}
#endif

void LFunc::autoRun(int isAutoRun, QString appName)
{
	QString appPath = QCoreApplication::applicationFilePath();

#ifdef _WIN32
	QString regPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	QSettings settings(regPath, QSettings::NativeFormat);
	if (isAutoRun)
		settings.setValue(appName, appPath);
	else
		settings.remove(appName);
#elif defined(__linux__)
    QString autostartPath = QDir::homePath() + "/.config/autoStart/";
	QString desktopFileName = "WindowsTop.desktop";
	QString desktopFileContent = QString(
		"[Desktop Entry]\n"
		"Type=Application\n"
		"Exec=%1\n"
		"Hidden=false\n"
		"NoDisplay=false\n"
		"X-GNOME-AutoStart-enabled=true\n"
		"Name[en_US]=WindowsTop\n"
	).arg(appPath);

	QDir autostartDir(autostartPath);
	if (!autostartDir.exists()) {
		QDir().mkpath(autostartPath); // Ensure the autoStart directory exists
	}

	QString desktopFilePath = autostartPath + desktopFileName;
	QFile desktopFile(desktopFilePath);

	if (isAutoRun) {
		if (desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QTextStream out(&desktopFile);
			out << desktopFileContent;
			desktopFile.close();
}
	}
	else {
		if (desktopFile.exists()) {
			QFile::remove(desktopFilePath);
		}
	}
#endif
}

QString LFunc::truncateString(const QString& str, int length)
{
	QString truncatedStr;
	if (str.length() <= length)
		truncatedStr = str;
	else
		QString truncatedStr = str.left(static_cast<qsizetype>(length) - 3) + "..."; // 留出三个位置给 "..."
	return truncatedStr;
}
