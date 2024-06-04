#include "LInIConfig.h"

using namespace ljz;

bool LInIConfig::read(const QVariant& key, QVariant& value)
{
	QSettings settings(this->path(), QSettings::IniFormat);
	value = settings.value(key.toString());
	return true;
}

bool LInIConfig::read(const QVariantList& keys, QVariant& value)
{
	return this->read(this->listToValue(keys), value);
}

QVariant LInIConfig::read(const QVariant& key)
{
	QVariant var;
	this->read(key, var);
	return var;
}

QVariant LInIConfig::read(const QVariantList& keys)
{
	return this->read(this->listToValue(keys));
}

bool LInIConfig::write(const QVariant& key, const QVariant& value, bool isAppend)
{
	QSettings settings(this->path(), QSettings::IniFormat);
	if (isAppend || settings.contains(key.toString()))
	{
		settings.setValue(key.toString(), value);
		return this->setError(NoError);
	}
	else
	{
		return this->setError(WriteError, QString("Key %1 not exist").arg(key.toString()));
	}
}

bool LInIConfig::write(const QVariantList& keys, const QVariant& value, bool isAppend)
{
	return this->write(this->listToValue(keys), value, isAppend);
}

QVariant LInIConfig::listToValue(const QVariantList& list)
{
	QStringList keyList;
	for (auto&& key : list)
		keyList.append(key.toString());
	return keyList.join("/");
}