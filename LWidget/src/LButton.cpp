#include "LButton.h"
#include <QPainterPath>
using namespace ljz;

void LSwitchButton::StateInfo::reSize(double radius)
{
	if (this->buttonPixmap.isNull())
		this->circlePixmap = QPixmap();
	else
	{
		const int size = static_cast<int>(radius * 2);
		if (this->circlePixmap.width() == size && this->circlePixmap.height() == size && !this->circlePixmap.isNull())
			return;
		QPixmap pixmap(size, size);
		QPixmap scaled = this->buttonPixmap.scaled(pixmap.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing);
		QPainterPath path;
		path.addEllipse(0, 0, size, size);
		painter.setClipPath(path);
		painter.drawPixmap(0, 0, scaled);
		this->circlePixmap = pixmap;
	}
}

LSwitchButton::StateInfo::StateInfo(State state)
{
	switch (state)
	{
	case State::On:
	{
		this->text = "On";
		this->buttonPixmap = QPixmap();
		this->backgroundColor = Qt::white;
		this->textColor = Qt::black;
		break;
	}
	case State::Off:
	{
		this->text = "Off";
		this->buttonPixmap = QPixmap();
		this->backgroundColor = Qt::black;
		this->textColor = Qt::white;
		break;
	}
	default:
		break;
	}
}

LSwitchButton::StateInfo::StateInfo(QString text, QPixmap buttonPixmap, const QColor& backgroundColor,
	const QColor& textColor) : text(std::move(text)), buttonPixmap(std::move(buttonPixmap)), backgroundColor(backgroundColor), textColor(textColor)
{}

void LSwitchButton::setStateInfo(State state, const StateInfo& stateInfo)
{
	this->_stateInfoMap.insert(state, stateInfo);
}

LSwitchButton::StateInfo LSwitchButton::stateInfo(State state) const
{
	return this->_stateInfoMap.value(state);
}

void LSwitchButton::setState(State state)
{
	this->setChecked(state == State::On);
}

LSwitchButton::State LSwitchButton::state() const
{
	return this->isChecked() ? State::On : State::Off;
}

void LSwitchButton::setAnimationDuration(int duration) const
{
	this->_animation->setDuration(duration);
}

int LSwitchButton::animationDuration() const
{
	return this->_animation->duration();
}

void LSwitchButton::setOpenAnimation(bool isOpenAnimation)
{
	this->_isOpenAnimation = isOpenAnimation;
}

bool LSwitchButton::isOpenAnimation() const
{
	return this->_isOpenAnimation;
}

LSwitchButton::LSwitchButton(QWidget* parent)
	:QAbstractButton(parent)
{
	this->setMouseTracking(true);
	this->setCheckable(true);
	connect(this, &QAbstractButton::clicked, this, &LSwitchButton::changeState);
	this->_animation->setDuration(100);
	connect(this->_animation, &QPropertyAnimation::finished, [this]()
		{
			this->updateValueRange();
			this->setPosition(this->_animation->startValue().toPointF());
		});
}

LSwitchButton::~LSwitchButton() = default;

void LSwitchButton::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	StateInfo info = this->_stateInfoMap.value(this->state());
	info.reSize(_radius);
	bool isAnimationRunning = this->_animation->state() == QAbstractAnimation::Running;
	double radius = this->calRadius(15) * 1.7;

	// 绘制背景
	painter.setPen(Qt::NoPen);
	painter.setBrush(info.backgroundColor);
	painter.drawRoundedRect(_buttonRect, radius, radius);

	// 如果动画运行中,绘制动画未移动到的区域背景
	if (isAnimationRunning)
	{
		double width = this->state() == State::On ? _buttonRect.bottomRight().x() - _position.x() : _position.x() - _buttonRect.topLeft().x();
		painter.setBrush(info.textColor);
		QRectF rect = QRectF(0, _buttonRect.top(), width, _buttonRect.height());
		if (this->state() == State::On)
			rect.moveLeft(_position.x());
		else
			rect.moveRight(_position.x());
		painter.drawRoundedRect(rect, radius, radius);
	}

	// 绘制按钮
	painter.setPen(info.textColor);
	if (info.circlePixmap.isNull())
	{
		painter.setBrush(isAnimationRunning ? info.backgroundColor : info.textColor);
		painter.drawEllipse(_position, _radius, _radius);
	}
	else
	{
		QRect pixmapRect = info.circlePixmap.rect();
		pixmapRect.moveCenter(_position.toPoint());
		painter.drawPixmap(pixmapRect, info.circlePixmap);
	}
	// 绘制文字
	QFontMetrics metrics(this->font());
	QRect textRect = metrics.boundingRect(info.text);
	textRect.moveCenter(_buttonRect.center().toPoint());
	painter.drawText(textRect, Qt::AlignCenter, info.text);
	// 绘制边框
	painter.setPen(info.textColor);
	painter.setBrush(Qt::NoBrush);
	painter.drawRoundedRect(_buttonRect, radius, radius);
	// 如果禁用，绘制遮罩
	if (!isEnabled())
	{
		painter.setBrush(QColor(128, 128, 128, 128));
		painter.setPen(Qt::NoPen);
		painter.drawRoundedRect(_buttonRect, radius, radius);
	}
}

void LSwitchButton::updateValueRange() const
{
	if (this->state() == State::On)
	{
		this->_animation->setStartValue(_right);
		this->_animation->setEndValue(_left);
	}
	else
	{
		this->_animation->setStartValue(_left);
		this->_animation->setEndValue(_right);
	}
}

void LSwitchButton::resizeEvent(QResizeEvent* event)
{
	QAbstractButton::resizeEvent(event);

	this->setRadius(this->calRadius(12));
	this->_left = QPointF(_radius + 5, this->height() / 2.0);
	this->_right = QPointF(this->width() - _radius - 5, this->height() / 2.0);
	this->_buttonRect = QRectF(this->rect().x() + 2, this->rect().y() + 2, this->rect().width() - 4, this->rect().height() - 4);
	this->updateValueRange();
	this->setPosition(this->_animation->startValue().toPointF());
}

QSize LSwitchButton::sizeHint() const
{
	return { 60, 23 };
}

void LSwitchButton::enterEvent(QEnterEvent* event)
{
	QAbstractButton::enterEvent(event);
}

void LSwitchButton::leaveEvent(QEvent* event)
{
	QAbstractButton::leaveEvent(event);
	if (this->isEnabled())
		this->setRadius(this->calRadius(12));
}

bool LSwitchButton::hitButton(const QPoint& pos) const
{
	return _buttonRect.contains(pos);
}

void LSwitchButton::mouseMoveEvent(QMouseEvent* event)
{
	QAbstractButton::mouseMoveEvent(event);
	if (this->isEnabled())
	{
		if (_buttonRect.contains(event->position()))
			this->setRadius(this->calRadius(10));
		else
			this->setRadius(this->calRadius(12));
	}
}

void LSwitchButton::setRadius(double radius)
{
	this->_radius = radius;
	update();
}

double LSwitchButton::calRadius(double num) const
{
	return qMin((this->width() - num) / 2.0, (this->height() - num) / 2.0);
}

void LSwitchButton::changeState(bool checked)
{
	State state = checked ? State::On : State::Off;
	emit stateChanged(state, this->_stateInfoMap.value(state));
	if (this->_isOpenAnimation)
		this->_animation->start();
	else
		this->setPosition(state == State::On ? _right : _left);
}

void LSwitchButton::setPosition(QPointF newValue)
{
	this->_position = newValue;
	update();
}