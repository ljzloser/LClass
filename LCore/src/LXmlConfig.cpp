#include "LXmlConfig.h"
using namespace ljz;
bool LXmlConfig::read(const QVariant& key, QVariant& value)
{
	return false;
}

bool LXmlConfig::read(const QVariantList& keys, QVariant& value)
{
	return false;
	QFile file;
	if (!this->getFile(file, LConfig::ReadOnly))
	{
		return this->setError(Error::OpenError, file.errorString());
	}
	else
	{
		QDomDocument doc;
		if (!doc.setContent(&file))
		{
			return this->setError(Error::ReadError, "This Config file " + file.errorString() + " is not a valid XML file.");
		}
		else
		{
			QDomElement root = doc.documentElement();
			if (root.isNull())
			{
				return this->setError(Error::ReadError, "This Config file " + file.errorString() + " is not a valid XML file.");
			}
			else
			{
				for (auto&& key : keys)
				{
					auto elements = root.elementsByTagName(key.toString());
					if (elements.size() == 0)
					{
						return this->setError(Error::ReadError, "This Config file " + file.errorString() + " is not a valid XML file.");
					}
					else if (elements.size() == 1)
					{
						root = elements.at(0).toElement();
					}
					else
					{
						root = elements.at(key.toInt()).toElement();
					}
					QStringList list;
					for (int i = 0; i < root.childNodes().size(); i++)
					{
						list.append(root.childNodes().at(i).toElement().text());
					}
					value = QVariant(list);
					return this->setError(Error::NoError);
				}
			}
		}
	}
}

QVariant LXmlConfig::read(const QVariant& key)
{
	return QVariant();
}

QVariant LXmlConfig::read(const QVariantList& keys)
{
	return QVariant();
}

bool LXmlConfig::write(const QVariant& key, const QVariant& value, bool isAppend)
{
	return false;
}

bool LXmlConfig::write(const QVariantList& keys, const QVariant& value, bool isAppend)
{
	return false;
}