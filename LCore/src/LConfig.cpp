#include "LConfig.h"

using namespace ljz;

LConfig::LConfig(const QString& path)
	: m_path(path), m_error(Error::NoError), m_errorString(QString())
{
}

LConfig::~LConfig()
{
}
QString LConfig::path() const
{
	return this->m_path;
}

void LConfig::setPath(const QString& path)
{
	this->m_path = path;
}

LConfig::Error LConfig::error() const
{
	return this->m_error;
}

QString LConfig::errorString() const
{
	return this->m_errorString;
}

bool LConfig::getFile(QFile& file, OpenMode mode)
{
	file.setFileName(this->m_path);
	QIODevice::OpenMode doMode = mode == ReadOnly ? QIODevice::ReadOnly : (QIODevice::WriteOnly | QIODevice::Text);
	if (!file.open(doMode))
		return this->setError(Error::OpenError, file.errorString());
	else
		return this->setError(Error::NoError, QString());
}

bool LConfig::setError(Error error, const QString& errorString)
{
	this->m_error = error;
	this->m_errorString = errorString;
	return error == Error::NoError;
}

bool LConfig::saveFile(const QByteArray& byte)
{
	QFile file;
	if (!this->getFile(file, LConfig::WriteOnly))
		return this->setError(Error::WriteError, file.errorString());
	file.write(byte);
	file.close();
	return this->setError(Error::NoError);
}