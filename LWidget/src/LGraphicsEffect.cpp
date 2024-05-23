#include "LGraphicsEffect.h"
#include <qpropertyanimation.h>
using namespace ljz;
LAnimationOpacityEffect::LAnimationOpacityEffect(QObject* parent)
	: QGraphicsOpacityEffect(parent)
{
	this->setOpacity(0.0);
}

LAnimationOpacityEffect::~LAnimationOpacityEffect()
= default;

void LAnimationOpacityEffect::inAnimationStart()
{
	QPropertyAnimation* fadeInAnimation = new QPropertyAnimation(this, "opacity");
	connect(fadeInAnimation, &QPropertyAnimation::finished, [=]() {emit this->inAnimationFinished(); });
	this->setAnimation(fadeInAnimation, 0.0, 1.0);
}

void LAnimationOpacityEffect::setAnimation(QPropertyAnimation* animation, const QVariant& begin, const QVariant& end) const
{
	animation->setDuration(this->_duration);
	animation->setStartValue(begin);
	animation->setEndValue(end);
	animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void LAnimationOpacityEffect::outAnimationStart()
{
	QPropertyAnimation* fadeOutAnimation = new QPropertyAnimation(this, "opacity");
	connect(fadeOutAnimation, &QPropertyAnimation::finished, [=]() {emit this->outAnimationFinished(); });
	this->setAnimation(fadeOutAnimation, 1.0, 0.0);
}