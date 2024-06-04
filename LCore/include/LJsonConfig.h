#pragma once
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include "LConfig.h"
#include "lcore_global.h"
namespace ljz
{
	class LCORE_EXPORT LJsonConfig : public LConfig
	{
	public:
		LJsonConfig(const QString& path);
		~LJsonConfig();
		/**
		 * @brief 初始化配置文件
		 * @param doc 配置文件的Json文档
		 * @return 是否初始化成功
		 */
		bool init(const QJsonDocument& doc);
		/**
		 * @brief 从配置文件中读取数据
		 * @param key 需要读取的 key
		 * @param value 读取到的数据
		 * @return 是否读取成功
		 */
		bool read(const QVariant& key, QVariant& value) override;
		/**
		 * @brief 使用迭代的方式从配置文件中读取数据
		 * @param keys 需要迭代读取的 keys
		 * @param value 读取到的数据
		 * @return 是否读取成功
		 */
		bool read(const QVariantList& keys, QVariant& value) override;
		/**
		 * @brief 从配置文件中读取数据
		 * @param key 需要读取的 key
		 * @return 读取到的数据
		 */
		QVariant read(const QVariant& key) override;
		/**
		 * @brief 使用迭代的方式从配置文件中读取数据
		 * @param keys 需要迭代读取的 keys
		 * @return 读取到的数据
		 */
		QVariant read(const QVariantList& keys) override;
		/**
		 * @brief 从配置文件中写入数据
		 * @param key 需要写入的 key
		 * @param value 需要写入的数据
		 * @param isAppend 是否追加 当为真时如果没有则追加该 key
		 * @return 是否写入成功
		 */
		bool write(const QVariant& key, const QVariant& value, bool isAppend = true) override;
		/**
		 * @brief 使用迭代的方式从配置文件中写入数据
		 * @param keys 需要迭代写入的 keys
		 * @param value 需要写入的数据
		 * @param isAppend 是否追加 当为真时如果没有则追加该 keys
		 * @return 是否写入成功
		 */
		bool write(const QVariantList& keys, const QVariant& value, bool isAppend = true) override;
		/**
		 * @brief 将JsonDocument 转为 QJsonValue
		 * @param doc QJsonDocument
		 * @param val QJsonValue
		 */
		void docToValue(QJsonDocument& doc, QJsonValue& val);
		/**
		 * @brief 从配置文件中读取QJsonDocument
		 * @return QJsonDocument
		 */
		QJsonDocument readJson();
		/**
		 * @brief 从配置文件中读取QJsonDocument
		 * @param doc 读取到的QJsonDocument
		 * @return 是否读取成功
		 */
		bool readJson(QJsonDocument& doc);
	};
}
