#pragma once
#include "lwidget_global.h"
#include <QAbstractButton>
#include <QPainter>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QEvent>
#include <QObject>
#include <QPushButton>
namespace ljz
{
	class LWIDGET_EXPORT LPixmapButton : public QPushButton
	{
		Q_OBJECT
	public:
		explicit LPixmapButton(QWidget* parent = nullptr);
		~LPixmapButton() override = default;
		void setPixmap(const QPixmap& pixmap);
		QPixmap pixmap() const;
	protected:
		void paintEvent(QPaintEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;
		void enterEvent(QEnterEvent* event) override;
		void leaveEvent(QEvent* event) override;
		QPixmap _pixmap;
		QPixmap _drawPixmap;
	};
	/**
	 * 开关按钮控件
	 */
	class LWIDGET_EXPORT LSwitchButton : public QAbstractButton
	{
		Q_OBJECT
			Q_PROPERTY(QPointF position READ position WRITE setPosition)
			Q_PROPERTY(State state READ state WRITE setState)
	public:

		/**
		 * 按钮状态
		 */
		enum class State
		{
			Off = 0,
			On = 1
		};

		/**
		 * 按钮状态信息，包含按钮文本、按钮图标、按钮圆形图标、按钮背景颜色、文本颜色
		 */
		struct StateInfo
		{
			QString text = QString();			// 文本
			QPixmap buttonPixmap = QPixmap(); 	// 按钮图标
			QColor backgroundColor = QColor();	// 背景颜色
			QColor textColor = QColor();		// 文本颜色
			QPixmap circlePixmap = QPixmap();	// 圆形图标 这里不需要设置值，内部会自动计算
			void reSize(double radius);			// 重新计算圆形图标
			explicit StateInfo(State state);
			StateInfo() = default;
			/**
			 *
			 * @param text 文本
			 * @param buttonPixmap 按钮图标
			 * @param backgroundColor 背景颜色
			 * @param textColor 文本颜色
			 */
			StateInfo(QString text, QPixmap buttonPixmap, const QColor& backgroundColor, const QColor& textColor);
		};
		// 根据状态设置状态信息
		void setStateInfo(State state, const StateInfo& stateInfo);
		// 根据状态获取状态信息
		[[nodiscard]] StateInfo stateInfo(State state) const;
		// 设置状态
		void setState(State state);
		// 获取状态
		[[nodiscard]] State state() const;
		// 设置动画持续时间
		void setAnimationDuration(int duration) const;
		// 获取动画持续时间
		[[nodiscard]] int animationDuration() const;
		// 设置是否开启动画
		void setOpenAnimation(bool isOpenAnimation);
		// 获取是否开启动画
		[[nodiscard]] bool isOpenAnimation() const;

		explicit LSwitchButton(QWidget* parent = nullptr);
		~LSwitchButton() override;
		void setObjectName(const QString& name);
	signals:
		// 状态改变
		void stateChanged(const State& state, const StateInfo& info);

	protected:
		void paintEvent(QPaintEvent* e) override;
		// 更新动画值范围
		void updateValueRange() const;
		void resizeEvent(QResizeEvent* event) override;
		QSize sizeHint() const override;
		void enterEvent(QEnterEvent* event) override;
		void leaveEvent(QEvent* event) override;
		bool hitButton(const QPoint& pos) const override;
		void mouseMoveEvent(QMouseEvent* event) override;
		// 设置按钮半径
		void setRadius(double radius);
		// 计算按钮半径
		double calRadius(double num) const;
	private:
		// 状态信息
		QMap<State, StateInfo> _stateInfoMap{ {State::Off, StateInfo(State::Off)}, {State::On, StateInfo(State::On)} };
		// 按钮位置
		QPointF _position{ 0,0 };
		// 动画指针
		QPropertyAnimation* _animation{ new QPropertyAnimation(this, "position") };
		// 按钮左极限
		QPointF _left{ 0,0 };
		// 按钮右极限
		QPointF _right{ 0,0 };
		// 按钮半径
		double _radius{ 0 };
		// 按钮整体矩形范围
		QRectF _buttonRect{ 0,0,0,0 };
		// 是否开启动画
		bool _isOpenAnimation{ true };
	private slots:
		// 改变状态
		void changeState(bool checked);
		// 设置按钮位置
		void setPosition(QPointF newValue);
		// 获取按钮位置
		[[nodiscard]] QPointF position() const { return _position; }
	};
}
