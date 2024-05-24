#include "func.h"
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
	return text.replace(" ", "&nbsp;");
}