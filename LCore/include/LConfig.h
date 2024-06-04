#pragma once
#include <QString>
#include <QDir>
#include <QFile>
#include "lcore_global.h"

namespace ljz
{
	class LCORE_EXPORT LConfig
	{
	public:
		enum Error // 配置文件错误
		{
			NoError = 0, // 没有错误
			OpenError = 1, // 打开配置文件失败
			ReadError = 2, // 读取配置文件失败
			WriteError = 3, // 写入配置文件失败
		};

		/**
		 * @brief 构造函数
		 * @param path 配置文件路径
		 */
		LConfig(const QString& path);

		~LConfig();
		/**
		 * @brief 读取配置文件
		 * @param key 键
		 * @param value 值
		 * @return 是否读取成功
		 */
		virtual bool read(const QVariant& key, QVariant& value) = 0;
		/**
		 * @brief 读取配置文件
		 * @param keys 键
		 * @param value 值
		 * @return 是否读取成功
		 */
		virtual bool read(const QVariantList& keys, QVariant& value) = 0;
		/**
		 * @brief 读取配置文件
		 * @param key 键
		 * @return 值
		 */
		virtual QVariant read(const QVariant& key) = 0;
		/**
		 * @brief 读取配置文件
		 * @param key 键
		 * @return 值
		 */
		virtual QVariant read(const QVariantList& keys) = 0;
		/**
		 * @brief 写入配置文件
		 * @param key 键
		 * @param value 值
		 * @param isAppend 是否追加
		 * @return 是否写入成功
		 */
		virtual bool write(const QVariant& key, const QVariant& value, bool isAppend = true) = 0;
		/**
		 * @brief 写入配置文件
		 * @param keys 键
		 * @param value 值
		 * @param isAppend 是否追加
		 * @return 是否写入成功
		 */
		virtual bool write(const QVariantList& keys, const QVariant& value, bool isAppend = true) = 0;

		/**
		 * @brief 获取配置文件的路径
		 * @return 配置文件的路径
		 */
		QString path() const;

		/**
		 * @brief 设置配置文件的路径
		 * @param path 配置文件的路径
		 */
		virtual	void setPath(const QString& path);

		/**
		 * @brief 获取错误的类型
		 * @return 错误的类型
		 */
		Error error() const;

		/**
		 * @brief 获取错误的字符串
		 * @return 错误的字符串
		 */
		QString errorString() const;
	protected:
		enum OpenMode // 打开模式
		{
			ReadOnly = 0, // 只读
			WriteOnly = 1, // 只写
		};

		/**
		 * @brief 获取QFile对象
		 * @param file QFile对象的引用
		 * @param mode 打开模式
		 * @return 是否打开成功
		 */
		bool getFile(QFile& file, OpenMode mode = ReadOnly);

		/**
		 * @brief 设置错误的原因
		 * @param error 错误类型
		 * @param errorString 错误的字符串
		 * @return 是否发生错误
		 */
		bool setError(Error error, const QString& errorString = QString());

		bool saveFile(const QByteArray& byte);

		QString m_path; // 配置文件路径
		Error m_error; // 错误类型
		QString m_errorString; // 错误的字符串
	};
}
