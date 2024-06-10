#pragma once
#include <qstring.h>
#include <qmutex.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qapplication.h>
#include <qdir.h>
#include "lcore_global.h"
/*
namespace ljz
{
	enum LogLeaver {
		Fatal = 0,
		Critical = 1,
		Warning = 2,
		Info = 3,
		Debug = 4,
	};

	static LogLeaver LogType = LogLeaver::Debug;
	static QString LogLeaverToString(LogLeaver type)
	{
		switch (type)
		{
		case LogLeaver::Fatal:
			return "Fatal";
		case LogLeaver::Critical:
			return "Critical";
		case LogLeaver::Warning:
			return "Warning";
		case LogLeaver::Info:
			return "Info";
		case LogLeaver::Debug:
			return "Debug";
		default:
			return "Debug";
		}
	}
	void createDir(QString& dirPath)
	{
		QDir dir(dirPath);
		if (!dir.exists())
			dir.mkpath(dirPath);
	}

	void message_output(QtMsgType type, const QMessageLogContext& context, const QString& msg)
	{
		//加锁：避免对文件的同时读写
		static QMutex mutex;
		mutex.lock();
		//输出的字符串
		QString strOutStream = "";
		//case 生成要求格式日志文件，加日志等级过滤
		QString outMsg = QString("[%1] [%2] [%3] [%4] [%5] \n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(QString(context.file)).arg(context.line).arg(QString(context.function)).arg(msg);
		switch (type) {
		case QtDebugMsg:
			if (LogType == Debug)
			{
				strOutStream = QString("[%1] %2").arg(LogLeaverToString(LogType)).arg(outMsg);
			}
			break;
		case QtInfoMsg:
			if (LogType >= Info)
			{
				strOutStream = QString("[%1] %2").arg(LogLeaverToString(LogType)).arg(outMsg);
			}
			break;
		case QtWarningMsg:
			if (LogType >= Warning)
			{
				strOutStream = QString("[%1] %2").arg(LogLeaverToString(LogType)).arg(outMsg);
			}
			break;
		case QtCriticalMsg:
			if (LogType >= Critical)
			{
				strOutStream = QString("[%1] %2").arg(LogLeaverToString(LogType)).arg(outMsg);
			}
			break;
		case QtFatalMsg:
			if (LogType >= Fatal)
			{
				strOutStream = QString("[%1] %2").arg(LogLeaverToString(LogType)).arg(outMsg);
			}
			abort();
		}
		QString dirPath = QApplication::applicationDirPath() + "/Log/";
		createDir(dirPath);
		//每天生成一个新的 log日志文件，文件名 yyyyMMdd.txt
		QString strFileName = dirPath + QString("%1.txt").arg(QDateTime::currentDateTime().date().toString("yyyyMMdd"));
		QFile logfile(strFileName);
		logfile.open(QIODevice::WriteOnly | QIODevice::Append);
		if (strOutStream != "")
		{
			QTextStream logStream(&logfile);
			logStream << strOutStream << "\r\n";
		}
		//清楚缓存文件，解锁
		logfile.flush();
		logfile.close();
		mutex.unlock();
	}
	void clear()
	{
		QDir directory(QApplication::applicationDirPath() + "/Log/");
		if (directory.exists())
		{
			for (QString entry : directory.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot))
				directory.remove(entry);
		}
	}
}
*/