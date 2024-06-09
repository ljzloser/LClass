#include "LSingleApplication.h"

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QFileInfo>
#include <QMessageBox>

LSingleApplication::LSingleApplication(int& args, char** argv)
	:QApplication(args, argv),
	instanceRunning(false),
	localServer(NULL),
	mainWindow(NULL)
{
	/**
	 * 一般来说是取引用程序名作为 localServer 的进程服务名
	 * 目前程序的进程名是根据文件名来的，但是一旦用于更改了文件名，那么进程名就不准确了，
	 * 所以通过文件名来判断是进程服务是不可靠的，应该写死在代码里
	 */
	serverName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
	//serverName = name;
	initLocalConnection();
}

void LSingleApplication::initLocalConnection()
{
	instanceRunning = false;
	QLocalSocket socket;
	//将套接字连接至进程服务，参数即为进程服务名
	socket.connectToServer(serverName);
	//若在500ms内连接至进程服务，说明进程服务已运行
	if (socket.waitForConnected(500)) {
		instanceRunning = true;

		// 其他处理，如：将启动参数发送到进程服务端
		QTextStream stream(&socket);
		QStringList args = QCoreApplication::arguments();
		if (args.count() > 1)
			stream << args.last();
		else
			stream << QString();
		stream.flush();
		socket.waitForBytesWritten();
		return;
	}
	newLocalServer();//连接不上进程服务器，就创建一个
}

// 说明：
// 创建LocalServer

void LSingleApplication::newLocalServer()
{
	localServer = new QLocalServer(this);
	connect(localServer, SIGNAL(newConnection()), this, SLOT(newLocalConnection()));
	if (!localServer->listen(serverName)) {
		// 此时监听失败，可能是程序崩溃时,残留进程服务导致的,移除之
		if (localServer->serverError() == QAbstractSocket::AddressInUseError) {
			QLocalServer::removeServer(serverName); //移除进程服务
			localServer->listen(serverName); //再次监听
		}
	}
}

void LSingleApplication::newLocalConnection()
{
	//监听进程服务，当有数据进来时，可以通过 nextPendingConnection() 获取套接字
	QLocalSocket* socket = localServer->nextPendingConnection();
	if (!socket)
		return;
	socket->waitForReadyRead(1000);
	//其他处理
	QTextStream stream(socket);
	delete socket;
	if (mainWindow != NULL) {
		//当主窗口对象不为空时，说明有可能主窗口被隐藏了，所以需要将其展示在桌面，同时给予用户提示
		QMessageBox::warning(NULL, "警告", QString("%1已经运行！").arg(serverName));
		mainWindow->raise();
		mainWindow->activateWindow();
		mainWindow->setWindowState((mainWindow->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
		mainWindow->show();
		QRect rect = mainWindow->geometry();
		rect.moveCenter(QCursor::pos());
		mainWindow->setGeometry(rect);
	}
}

// 说明：
// 检查是否已有一个实例在运行, true - 有实例运行， false - 没有实例运行

bool LSingleApplication::getInstanceRunning() const
{
	return instanceRunning;
}

QWidget* LSingleApplication::getMainWindow() const
{
	return mainWindow;
}

void LSingleApplication::setMainWindow(QWidget* value)
{
	mainWindow = value;
}