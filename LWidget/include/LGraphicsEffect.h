#pragma once
#include "lwidget_global.h"
#include <QGraphicsOpacityEffect>
#include <qpropertyanimation.h>
#include <qtmetamacros.h>
#include <qvariant.h>

namespace ljz
{
	class LWIDGET_EXPORT LAnimationOpacityEffect : public QGraphicsOpacityEffect
	{
		Q_OBJECT
	signals:
		void inAnimationFinished();
		void outAnimationFinished();

	public:
		explicit LAnimationOpacityEffect(QObject* parent);
		~LAnimationOpacityEffect() override;
		void setDuration(int duration) { this->_duration = duration; }
		[[nodiscard]] int duration()const { return  this->_duration; }
		void inAnimationStart();
		/**
		 * @brief 设置动画的开始和结束
		 * @param animation 属性动画指针
		 * @param begin 开始
		 * @param end 结束
		*/
		void setAnimation(QPropertyAnimation* animation, const QVariant& begin, const QVariant& end) const;
		/**
		 * @brief 结束动画开始
		*/
		void outAnimationStart();
		/*
		 * @brief 设置动画的开始和结束的值
		 */
		void setRange(const QVariant& begin, const QVariant& end);
		/*
		 * @brief 获取动画的开始和结束的值
		 */
		std::pair<QVariant, QVariant> range()const;
	private:
		QVariant _begin{ QVariant(0.0) };
		QVariant _end{ QVariant(0.1) };
		int _duration = 1000;
	};
}