#include "LJsonConfig.h"
using namespace ljz;
LJsonConfig::LJsonConfig(const QString& path)
	: LConfig(path)
{
}

LJsonConfig::~LJsonConfig()
{
}

bool LJsonConfig::init(const QJsonDocument& doc)
{
	return this->saveFile(doc.toJson());
}

bool LJsonConfig::read(const QVariant& key, QVariant& value)
{
	return this->read(QVariantList() << key, value);
}

bool LJsonConfig::read(const QVariantList& keys, QVariant& value)
{
	QJsonDocument doc;
	QJsonValue val;
	if (this->readJson(doc))
	{
		this->docToValue(doc, val);

		for (auto&& key : keys)
		{
			if (val.isObject())
				val = val.toObject().value(key.toString());
			else if (val.isArray())
				val = val.toArray().at(key.toInt());
			else
				return this->setError(Error::ReadError,
					QString("Json document is not an object or array at value %1").arg(key.toString()));
		}
	}
	else
	{
		return false;
	}
	value = val.toVariant();
	return this->setError(Error::NoError);
}

QVariant LJsonConfig::read(const QVariant& key)
{
	QVariant var;
	this->read(key, var);
	return var;
}

QVariant LJsonConfig::read(const QVariantList& keys)
{
	QVariant var;
	this->read(keys, var);
	return var;
}

bool LJsonConfig::write(const QVariant& key, const QVariant& value, bool isAppend)
{
	return this->write(QVariantList() << key, value, isAppend);
}

bool LJsonConfig::write(const QVariantList& keys, const QVariant& value, bool isAppend)
{
	QJsonDocument doc; // JSON文档对象
	QJsonValue val; // JSON值对象
	if (this->readJson(doc)) // 读取JSON文档
	{
		this->docToValue(doc, val); // 将文档转换为值
		QList<QJsonValue> array; // JSON值列表
		array.append(val); // 将值添加到列表
		for (auto&& key : keys) // 遍历键
		{
			QJsonValue newVal = array.last(); // 获取最后一个值
			if (newVal.isObject()) // 如果值是对象
			{
				QJsonObject obj = newVal.toObject(); // 获取对象
				if (isAppend && !obj.contains(key.toString())) // 如果是追加且键不存在
					obj.insert(key.toString(), QJsonValue(QJsonObject())); // 插入空对象值
				else if (!isAppend && !obj.contains(key.toString())) // 如果不是追加且键不存在
					return this->setError(Error::WriteError, QString("This is object but key %1 not exist").arg(key.toString())); // 返回写入错误
				newVal = obj.value(key.toString()); // 获取新值
			}
			else if (newVal.isArray()) // 如果值是数组
			{
				QJsonArray arr = newVal.toArray(); // 获取数组
				if (isAppend && arr.size() == key.toInt()) // 如果是追加且索引越界一位
					arr.push_back(QJsonValue(QJsonObject())); // 添加空对象值
				else if (key.toInt() > arr.size() || (!isAppend && arr.size() == key.toInt()))
					return this->setError(Error::WriteError, QString("This is array but index %1 not exist or can't append index").arg(key.toString())); // 返回写入错误
				newVal = arr.at(key.toInt()); // 获取新值
			}
			else // 值既不是对象也不是数组
			{
				return this->setError(Error::WriteError,
					QString("JsonValue is not object or array，value is %1").arg(newVal.toVariant().toString())); // 返回写入错误
			}
			array.append(newVal); // 将新值添加到列表
		}
		array.last() = value.toJsonValue(); // 设置最后一个值为传入值的JSON表示
		for (int i = array.size() - 2; i >= 0; i--) // 从倒数第二个值开始往前遍历
		{
			if (array.at(i).isObject()) // 如果值是对象
			{
				QJsonObject obj = array.at(i).toObject(); // 获取对象
				obj.insert(keys.at(i).toString(), array.at(static_cast<qsizetype>(i) + 1)); // 插入键值对
				array.replace(i, QJsonValue(obj)); // 替换列表中对应位置的值
			}
			else if (array.at(i).isArray()) // 如果值是数组
			{
				QJsonArray arr = array.at(i).toArray(); // 获取数组
				if (arr.size() <= keys.at(i).toInt()) // 如果索引越界
					arr.push_back(QJsonValue(QJsonObject())); // 添加空对象值
				arr[keys.at(i).toInt()] = array.at(static_cast<qsizetype>(i) + 1); // 设置数组中的值
				array[i] = QJsonValue(arr); // 替换列表中对应位置的值
			}
		}
		QByteArray bytes; // 字节数组用于保存JSON文档的字节
		if (array.first().isObject()) // 如果第一个值是对象
			bytes = QJsonDocument(array.first().toObject()).toJson(); // 将对象转换为JSON字节数组
		else if (array.first().isArray()) // 如果第一个值是数组
			bytes = QJsonDocument(array.first().toArray()).toJson(); // 将数组转换为JSON字节数组
		return this->saveFile(bytes); // 保存字节数组到文件
	}
	else
	{
		return false; // 返回失败
	}
}

void LJsonConfig::docToValue(QJsonDocument& doc, QJsonValue& val)
{
	if (doc.isObject())
		val = QJsonValue(doc.object());
	else if (doc.isArray())
		val = QJsonValue(doc.array());
	else
		val = QJsonValue();
}

QJsonDocument LJsonConfig::readJson()
{
	QJsonDocument doc;
	this->readJson(doc);
	return doc;
}

bool LJsonConfig::readJson(QJsonDocument& doc)
{
	QFile file;
	if (this->getFile(file, LJsonConfig::ReadOnly))
	{
		doc = QJsonDocument::fromJson(file.readAll());
		file.close();
		if (doc.isNull() || doc.isEmpty())
			return this->setError(Error::ReadError, QString("This Config file %1 is not a valid JSON file.").arg(this->m_path));
		else
			return this->setError(Error::NoError);
	}
	return false;
}