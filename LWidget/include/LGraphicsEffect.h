#pragma once
#include "lwidget_global.h"
#include <QGraphicsOpacityEffect>
#include <qpropertyanimation.h>
#include <qtmetamacros.h>
#include <qvariant.h>

namespace ljz
{
	class LWIDGET_EXPORT LAnimationOpacityEffect final : public QGraphicsOpacityEffect
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
	private:

		int _duration = 1000;
	};
}