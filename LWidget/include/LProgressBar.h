#pragma once
#include "lwidget_global.h"
#include "QWidget"
#include <QPropertyAnimation>
#include <QTimer>
namespace ljz
{
	/**
	 * This is the base class for all progress bars.\n
	 * 这是所有进度条的基类\n
	 * If you need to inherit.\n
	 * 如果你需要继承\n
	 * you need to implement the two pure virtual functions paintEvent and sizeHint\n
	 * 你需要实现两个纯虚函数 paintEvent 和 sizeHint
	 */
    class LWIDGET_EXPORT LBaseProgressBar  : public QWidget
	{
		Q_OBJECT
			Q_PROPERTY(qint32 minimum READ minimum WRITE setMinimum)
			Q_PROPERTY(qint32 maximum READ maximum WRITE setMaximum)
			Q_PROPERTY(QString text READ text)
			Q_PROPERTY(qint32 value READ value WRITE setValue NOTIFY valueChanged)
	public:
		[[nodiscard]] virtual int minimum() const;		// get the minimum value 获取最小值
		[[nodiscard]] virtual int maximum() const;		// get the maximum value 获取最大值
		[[nodiscard]] virtual int value() const;		// get the current value 获取当前值
		[[nodiscard]] virtual QString format() const;	// get the text format 获取进度文本格式化字符串
		[[nodiscard]] virtual QString text() const;		// get the text 获取进度文本
		[[nodiscard]] virtual double percent() const;	// get the percent 获取进度百分比

		explicit LBaseProgressBar(QWidget* parent = nullptr)
			: QWidget(parent)
		{
		}
		explicit LBaseProgressBar(const int minimum, const int maximum, QWidget* parent = nullptr)
			: QWidget(parent), _minimum(minimum), _maximum(maximum)
		{}
		explicit LBaseProgressBar(const LBaseProgressBar* other, QWidget* parent = nullptr)
			:QWidget(parent), _minimum(other->minimum()), _maximum(other->maximum()),
			_value(other->value()), _format(other->format())
		{}
		~LBaseProgressBar() override = default;
		/**
		 * 克隆一个 From 类型的对象。该函数默认不会析构 other。\n
		 * 如果 other 为 nullptr，返回一个指向 To 类型默认构造函数的指针。\n
		 * 不为 nullptr，则返回一个指向 To 类型的指针，并复制 other 的成员变量值。\n
		 * @tparam To LBaseProgressBar 的派生类
		 * @tparam From LBaseProgressBar 的派生类
		 * @param other From 类型的指针
		 * @param isDelete 是否要析构 other
		 * @return 返回一个指向 To 类型的指针
		 */
		template <typename To, typename From>
		static To* clone(From* other, const bool isDelete = false)
		{
			static_assert(std::is_base_of_v<LBaseProgressBar, To>, "To is not LBaseProgressBar");
			static_assert(std::is_base_of_v<LBaseProgressBar, From>, "From is not LBaseProgressBar");
			To* ret = new To();
			if (other)
			{
				ret->setParent(static_cast<QWidget*>(other->parent()));
				ret->setRange(other->minimum(), other->maximum());
				ret->setValue(other->value());
				ret->setFormat(other->format());
				ret->setObjectName(other->objectName());
				if (isDelete)
					delete other;
			}
			return ret;
		}
	public slots:
		// set the minimum value 设置最小值
		virtual void setMinimum(const int minimum);
		// set the maximum value 设置最大值
		virtual void setMaximum(const int maximum);
		/**
		 * set the range in minimum and maximum \n
		 * 设置最小值和最大值
		 * @param minimum
		 * @param maximum
		 */
		virtual void setRange(const int minimum, const int maximum);
		// set the current value 设置当前值
		virtual void setValue(const int value);

		/**
		 * %v: value, %p: percent, %m: maximum\n
		 * %v: 当前值, %p: 百分比, %m: 最大值\n
		 * example: setFormat("%v/%m\n%p%")\n
		 * 示例: setFormat("%v/%m\n%p%")\n
		 * default: "%p%"\n
		 * 默认进度文本格式为"%p%"
		 * @param format
		 */
		virtual void setFormat(const QString& format = "%p%");
		// add the value offset 增加值
		virtual void addValue(const int offset = 1);
	signals:
		// value changed signal 值改变信号
		void valueChanged(int value);
	protected:
		void paintEvent(QPaintEvent* event) override = 0;
		QSize sizeHint() const override = 0;
		int _minimum{ 0 };
		int _maximum{ 0 };
		int _value{ 0 };
		QString _format{ "%p%" };
	};

	/**
	 * This is the base class for all circular progress bars.\n
	 * 这是所有圆形进度条的基类\n
	 * only Implemented the LBaseProgressBar Class Pure virtual function sizeHint\n
	 * 只实现了 LBaseProgressBar 类的纯虚函数 sizeHint
	 * If you need to inherit.\n
	 * 如果你需要继承\n
	 * you need to implement the one pure virtual function paintEvent
	 * 你需要实现一个纯虚函数 paintEvent
	 */
	class LWIDGET_EXPORT LCircularBaseProgressBar : public LBaseProgressBar
	{
		Q_OBJECT
	public:
		explicit LCircularBaseProgressBar(QWidget* parent = nullptr)
			:LBaseProgressBar(parent)
		{
		}
		explicit LCircularBaseProgressBar(const int minimum, const int maximum, QWidget* parent = nullptr)
			:LBaseProgressBar(minimum, maximum, parent)
		{}
		explicit LCircularBaseProgressBar(const LBaseProgressBar* other, QWidget* parent = nullptr)
			:LBaseProgressBar(other, parent)
		{}
	protected:
		QSize sizeHint() const override;
	};

	/**
	 * This is a progress bar with wave effect.\n
	 * 这是带波纹效果的进度条\n
	 */
	class LWIDGET_EXPORT LWaveProgressBar final : public LCircularBaseProgressBar
	{
		Q_OBJECT
			Q_PROPERTY(qreal wavePhase READ wavePhase WRITE setWavePhase)
	public:
		struct ColorInfo
		{
			QBrush backGroundBrush = QColor(Qt::white);
			QColor foreground = QColor(Qt::black);
			QBrush waveBrush = QColor(Qt::cyan);
			ColorInfo() = default;
		};
		explicit LWaveProgressBar(QWidget* parent = nullptr)
			:LCircularBaseProgressBar(parent)
		{
		}
		explicit LWaveProgressBar(const int minimum, const int maximum, QWidget* parent = nullptr)
			:LCircularBaseProgressBar(minimum, maximum, parent)
		{}
		explicit LWaveProgressBar(const LBaseProgressBar* other, QWidget* parent = nullptr)
			:LCircularBaseProgressBar(other, parent)
		{}
		void setWavePhase(const qreal phase) { this->_wavePhase = phase; this->update(); }
		[[nodiscard]] qreal wavePhase() const { return _wavePhase; }
		/**
		 * set the wave animation duration in millisecond default: 600
		 * 设置波纹动画时长，单位毫秒
		 * @param duration 0 to 2 * PI 从0到2*PI的时间间隔
		 */
		void setAnimationDuration(const int duration = 600) { _animationDuration = duration; }
		int animationDuration() const { return _animationDuration; }
		void setColorInfo(const ColorInfo& colorInfo) { _colorInfo = colorInfo; }
		ColorInfo colorInfo() const { return _colorInfo; }
	protected:
		void paintEvent(QPaintEvent* event) override;
		void showEvent(QShowEvent* event) override;
		void hideEvent(QHideEvent* event) override;
	private:
		qreal _wavePhase{ 0 };
		int _animationDuration{ 600 };
		QPropertyAnimation* _waveAnimation{ new QPropertyAnimation(this, "wavePhase") };
		ColorInfo _colorInfo;
	};

	/**
	 * This is a progress bar with ring effect.\n
	 * 这是环形效果的进度条
	 */
	class LWIDGET_EXPORT LRingProgressBar final : public LCircularBaseProgressBar
	{
	public:
		struct ColorInfo
		{
			QBrush backGroundBrush = QColor(200, 200, 200);
			QColor foreground = QColor(Qt::black);
			QBrush bodyBrush = QColor(0, 150, 150);
			ColorInfo() = default;
		};

		explicit LRingProgressBar(QWidget* parent = nullptr)
			:LCircularBaseProgressBar(parent)
		{}
		explicit LRingProgressBar(const int minimum, const int maximum, QWidget* parent = nullptr)
			:LCircularBaseProgressBar(minimum, maximum, parent)
		{}
		explicit LRingProgressBar(const LBaseProgressBar* other, QWidget* parent = nullptr)
			:LCircularBaseProgressBar(other, parent)
		{}
		~LRingProgressBar() override = default;
		void setColorInfo(const ColorInfo& colorInfo) { _colorInfo = colorInfo; }
		[[nodiscard]] ColorInfo colorInfo() const { return _colorInfo; }

	protected:
		/**
		 * get Arc Start and End Point\n
		 * 获取弧形的起点和终点坐标
		 * @param size
		 * @param start out
		 * @param end out
		 */
		void getArcStartEnd(int size, QPointF& start, QPointF& end) const;
		void paintEvent(QPaintEvent* event) override;
		ColorInfo _colorInfo;
	};


}
