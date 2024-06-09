#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QObject>
#include <QApplication>
#include "lcore_global.h"
class QMainWindow;
class QLocalServer;

class LCORE_EXPORT LSingleApplication : public QApplication
{
	Q_OBJECT
public:
	LSingleApplication(int& args, char** argv);
	bool getInstanceRunning() const; // 是否已经有实例在运行

	QWidget* getMainWindow() const;

	void setMainWindow(QWidget* value);

private slots:
	void newLocalConnection();

private:
	void initLocalConnection();  // 初始化本地连接
	void newLocalServer();       // 创建服务端
	bool instanceRunning;        // 是否已经有实例在运行
	QLocalServer* localServer;   // 本地socket Server（用于本地进程间通信）
	QString serverName;          // 服务名称
	QWidget* mainWindow;  // MainWindow指针
};

#endif // SINGLEAPPLICATION_H
