#pragma once

#include "lwidget_global.h"
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QTime>
#include <QTimer>
#include <QWidget>
namespace ljz
{
	class LWIDGET_EXPORT LInstrumentBoard : public QWidget
	{
		Q_OBJECT
	public:
		enum ColorKey
		{
			Background,//背景色
			MaxCir,//外圆
			MidCir,//中圆
			Range,//范围
			MinCir,//内圆
			Scale,//刻度
			Number,//数字
			CenterCir,//中心圆
			Ptr,//指针
			Value,//值
			Name//名称
		};
	signals:
		/**
		 * @brief 值改变信号
		 * @param double value 当前值
		 * @return
		 */
		void valueChange(double value);

	private:
		double _minValue; // 最小值
		double _maxValue; // 最大值
		int _precision = 0; // 精度
		double _value; //当前值
		double _drawValue = 0; // 当前绘制值
		QMap<ColorKey, QColor> _colorMap; // 颜色字典
		QString _unit; // 单位
		QTimer* _timer = new QTimer(this); //动画执行定时器
		int _timeChange = 20; // 动画更新时间毫秒
		QString _name; // 仪表名称
		bool _isOpenAnimation = true; // 是否开启动画

	public:
		/**
		 * @brief 小数构造
		 * @param minValue 最小值
		 * @param maxValue 最大值
		 * @param precision 精度
		 * @param parent 父类 默认为空指针
		 */
		LInstrumentBoard(double minValue, double maxValue, int precision, QWidget* parent = nullptr);
		/**
		 * @brief 整数构造
		 * @param minValue 最小值
		 * @param maxValue 最大值
		 * @param parent 父类 默认为空指针
		 */
		LInstrumentBoard(int minValue, int maxValue, QWidget* parent = nullptr);

		~LInstrumentBoard() override;
		/**
		 * @brief 设置小数范围
		 * @param minValue 最小值
		 * @param maxValue 最大值
		 * @param precision 精度
		 */
		void setRange(double minValue, double maxValue, int precision);
		/**
		 * @brief 设置整数范围
		 * @param minValue 最小值
		 * @param maxValue 最大值
		 */
		void setRange(int minValue, int maxValue);
		/**
		 * @brief 设置颜色
		 * @param key 颜色类型 具体见该枚举。
		 * @param color 颜色
		 */
		void setColor(ColorKey key, QColor color);
		/**
		 * @brief 设置颜色
		 * @param colorMap 颜色Map
		 */
		void setColor(QMap<ColorKey, QColor>& colorMap);
		/**
		 * @brief 设置整数值
		 * 会触发值改变信号。
		 * @param value 值
		 */
		void setValue(int value);
		/**
		 * @brief 设置小数值
		 * 会触发值改变信号。
		 * @param value 值
		 */
		void setValue(double value);
		/**
		 * @brief 设置动画更新时间
		 * 小于1ms则为1ms。
		 * @param time 时间ms
		 */
		void setTimeChange(int time) { this->_timeChange = time < 1 ? 1 : time; }
		/**
		 * @brief 设置单位
		 * 用于仪表盘上值后面显示的单位,可用不设置。
		 * @param unit 单位
		 */
		void setUnit(QString unit);
		/**
		 * @brief 设置名称
		 * 用于定义该仪表盘的名称，注意不是ObjectName。
		 * 是在仪表盘上显示的名称。
		 * @param unit 单位
		 */
		void setName(QString Name);
		/**
		 * @brief 设置是否开启动画
		 * 默认开启
		 * @param isOpen 为真开启
		 */
		void setOpenAnimation(bool isOpen);

		// 获取最小值
		[[nodiscard]] double getMinValue() const;
		// 获取最大值
		[[nodiscard]] double getMaxValue() const;
		// 获取精度
		[[nodiscard]] int getPrecision() const;
		// 获取值
		[[nodiscard]] double getValue() const;
		// 获取颜色Map
		const QMap<ColorKey, QColor>& getColor();
		// 获取单位
		[[nodiscard]] QString getUnit() const;
		/**
		 * @brief 获取颜色
		 * @param key ColorKey枚举
		 * @return QColor
		 */
		[[nodiscard]] QColor getColor(ColorKey key) const;
		// 获取名称
		[[nodiscard]] QString getName() const { return this->_name; }
		// 是否开启动画
		[[nodiscard]] bool isOpenAnimation() const { return this->_isOpenAnimation; }

	private:
		// 绘画事件
		void paintEvent(QPaintEvent* e) override;
		// 右键菜单事件
		void contextMenuEvent(QContextMenuEvent* e) override;
		// 初始化
		void init();
		// 值转角度
		[[nodiscard]] double valueToAngle() const;
		// 角度转值
		[[nodiscard]] double angleToValue(int angle) const;
	private slots:
		// 定时器槽函数，用于执行动画。
		void timeOutSlot();
	};
}