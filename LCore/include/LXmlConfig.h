#pragma once
#include "LConfig.h"
#include <qvariant.h>

#include <QtXml/QtXml>

namespace ljz
{
	class LXmlConfig :
		public LConfig
	{
	public:
		LXmlConfig(const QString& path) :LConfig(path) {};

		~LXmlConfig() {};

		/**
		 * @brief 读取配置文件
		 * @param key 键
		 * @param value 值
		 * @return 是否读取成功
		 */
		bool read(const QVariant& key, QVariant& value) override;
		/**
		 * @brief 读取配置文件
		 * @param keys 键
		 * @param value 值
		 * @return 是否读取成功
		 */
		bool read(const QVariantList& keys, QVariant& value) override;
		/**
		 * @brief 读取配置文件
		 * @param key 键
		 * @return 值
		 */
		QVariant read(const QVariant& key) override;
		/**
		 * @brief 读取配置文件
		 * @param key 键
		 * @return 值
		 */
		QVariant read(const QVariantList& keys) override;
		/**
		 * @brief 写入配置文件
		 * @param key 键
		 * @param value 值
		 * @param isAppend 是否追加
		 * @return 是否写入成功
		 */
		bool write(const QVariant& key, const QVariant& value, bool isAppend = true) override;
		/**
		 * @brief 写入配置文件
		 * @param keys 键
		 * @param value 值
		 * @param isAppend 是否追加
		 * @return 是否写入成功
		 */
		bool write(const QVariantList& keys, const QVariant& value, bool isAppend = true) override;
	};
}
