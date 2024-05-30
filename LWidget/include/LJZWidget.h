#pragma once

#include "lwidget_global.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QStyle>
#include <QWidget>
#include <QEvent>
#include <QSpacerItem>
#include <Windows.h>
#pragma comment (lib,"user32.lib")

namespace ljz
{
	class LWIDGET_EXPORT LBaseTitleBar : public QWidget
	{
		Q_OBJECT
	public:
		explicit LBaseTitleBar(QWidget* parent = nullptr) :QWidget(parent) {}
		~LBaseTitleBar() override = default;
		virtual QWidget* getMaxButton() = 0;

	signals:
		void closeButtonClicked();
		void minButtonClicked();
	public slots:
		virtual void updateIcon() = 0;
		virtual void closeButtonClick() = 0;
		virtual void minButtonClick() = 0;
	};

	class LWIDGET_EXPORT LTitleBar : public LBaseTitleBar
	{
		Q_OBJECT
	public:
		//按钮图标枚举
		enum ButtonIcon
		{
			MinButtonIcon,//最小化按钮图标
			MaxButtonIcon,//最大化按钮图标
			RestoreButtonIcon,//还原按钮图标
			CloseButtonIcon//关闭按钮图标
		};
		explicit LTitleBar(QWidget* parent = nullptr);
		~LTitleBar() override = default;
		//设置标题栏图标
		void setTitleIcon(const QIcon& icon);
		void setTitleIcon(const QPixmap& pixmap);
		//设置标题栏文本
		void setTitleText(const QString& text);
		//获取标准按钮图标
		QMap<ButtonIcon, QIcon> getStandardIconMap();
		//设置最小化按钮图标
		QIcon setMinButtonIcon(const QIcon& icon);
		QIcon setMinButtonIcon(const QString& iconPath);
		//设置最大化按钮图标
		QIcon setMaxButtonIcon(const QIcon& icon);
		QWidget* getMaxButton() override { return this->maxButton; }
		QIcon setMaxButtonIcon(const QString& iconPath);
		//设置还原按钮图标
		QIcon setRestoreButtonIcon(const QIcon& icon);
		QIcon setRestoreButtonIcon(const QString& iconPath);
		//设置关闭按钮图标
		QIcon setCloseButtonIcon(const QIcon& icon);
		QIcon setCloseButtonIcon(const QString& iconPath);

		void paintEvent(QPaintEvent* event) override;
		void closeButtonClick() override;
		void minButtonClick() override;

	protected:
		void mouseDoubleClickEvent(QMouseEvent* event) override;

	public slots:
		void updateIcon() override; //更新按钮图标

	private:
		QHBoxLayout* layout; //标题栏布局
		QIcon icon; //标题栏图标
		QString title; //标题栏文本
		QPushButton* minButton; //最小化按钮
		QPushButton* maxButton; //最大化按钮
		QPushButton* closeButton; //关闭按钮
		bool flag; //最大化按钮标志
		QMap<ButtonIcon, QIcon> standardIconMap; //标准按钮图标
	};
	class LWIDGET_EXPORT LWidget : public QWidget
	{
		Q_OBJECT
	public:
		struct Info
		{
			int edgeSize = 10;
			int radius = 10;
			QColor borderColor = Qt::GlobalColor::gray;
			int borderSize = 2;
			QColor backgroundColor = Qt::GlobalColor::white;
			QPixmap backgroundPixmap = QPixmap();
			QColor splitLineColor = Qt::GlobalColor::gray;
			Info() = default;
		};
		explicit LWidget(LBaseTitleBar* titleBar, QWidget* mainWidget, QWidget* parent = nullptr);
		~LWidget() override = default;
	public:
		void loadLayout() const;
		void showCustomNormal();
		void setTitleBar(LBaseTitleBar* titleBar);
		void setMenuBar(QWidget* menuBar);
		void setMainWidget(QWidget* mainWidget);
		void setStatusBar(QWidget* statusBar);
		void setInfo(const Info& info);
		Info info() const;
		LBaseTitleBar* getTitleBar() const { return this->_titleBar; }
		//获取菜单栏
		QWidget* getMenuBar() const { return this->_menuBar; }
		//获取主窗口
		QWidget* getMainWidget() const { return this->_mainWidget; }
		//获取状态栏
		QWidget* getStatusBar() const { return this->_statusBar; }
		void paintEvent(QPaintEvent* event) override;
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
		bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#else
		bool nativeEvent(const QByteArray& eventType, void* message, long* result);
#endif
		bool eventFilter(QObject* obj, QEvent* event) override;
	signals:
		void windowStateChanged();
		/**
		 * @brief 系统设置改变
		*/
		void systemSettingsChanged();
	protected:
		LBaseTitleBar* _titleBar = nullptr; //标题栏
		QWidget* _menuBar = nullptr; //菜单栏
		QWidget* _mainWidget = nullptr; //主窗口
		QWidget* _statusBar = nullptr; //状态栏
		QVBoxLayout* _layout = new QVBoxLayout(this);
		Info _info;
	};
}
