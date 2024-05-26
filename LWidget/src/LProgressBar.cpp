#include "LProgressBar.h"
#include <QPainter>
#include <QPainterPath>
using namespace ljz;

int LBaseProgressBar::minimum() const
{
	return _minimum;
}

int LBaseProgressBar::maximum() const
{
	return _maximum;
}

int LBaseProgressBar::value() const
{
	return _value;
}

QString LBaseProgressBar::format() const
{
	return _format;
}

QString LBaseProgressBar::text() const
{
	QString result = _format;
	result.replace("%p", QString("%1").arg(this->percent(), 0, 'f', 2));
	result.replace("%v", QString::number(_value));
	result.replace("%m", QString::number(_maximum));
	return result;
}

double LBaseProgressBar::percent() const
{
	return _value * 100.0 / _maximum;
}

void LBaseProgressBar::setMinimum(const int minimum)
{
	_minimum = minimum;
}

void LBaseProgressBar::setMaximum(const int maximum)
{
	_maximum = maximum;
}

void LBaseProgressBar::setRange(const int minimum, const int maximum)
{
	_minimum = minimum; _maximum = maximum;
}

void LBaseProgressBar::setValue(const int value)
{
	if (value < _minimum)
	{
		_value = _minimum;
	}
	else if (value > _maximum)
	{
		_value = _maximum;
	}
	else
	{
		_value = value;
		emit valueChanged(_value);
	}
	this->update();
}

void LBaseProgressBar::setFormat(const QString& format)
{
	_format = format;
}

void LBaseProgressBar::addValue(const int offset)
{
	this->setValue(_value + offset);
}

void LRingProgressBar::getArcStartEnd(int size, QPointF& start, QPointF& end) const
{
	int x = -size; // 矩形左上角的 x 坐标
	int y = -size; // 矩形左上角的 y 坐标
	int width = size * 2; // 矩形的宽度
	int height = size * 2; // 矩形的高度
	int startAngle = 0; // 圆弧的起始角度
	int spanAngle = 360 * 16 * this->percent() / 100; // 圆弧的角度跨度

	// 计算圆弧的中心点坐标
	QPointF center(x + width / 2.0, y + height / 2.0);
	qreal radius = qMin(width, height) / 2.0; // 圆弧的半径

	// 将起始角度和角度跨度转换为弧度
	qreal startRadians = qDegreesToRadians(startAngle);
	qreal spanRadians = qDegreesToRadians(spanAngle / 16.0);

	// 计算起点和终点的极坐标
	qreal startX = center.x() + radius * qCos(startRadians);
	qreal startY = center.y() - radius * qSin(startRadians);
	qreal endX = center.x() + radius * qCos(startRadians + spanRadians);
	qreal endY = center.y() - radius * qSin(startRadians + spanRadians);

	// 构造起点和终点的坐标
	start = { startX, startY };
	end = { endX, endY };
}

void LRingProgressBar::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.save();
	painter.translate(this->rect().center());
	painter.rotate(-90);
	int size = this->sizeHint().width() / 2;

	// 外环
	QPainterPath path;
	path.moveTo(0, -size);
	QRect rect(-size, -size, size * 2, size * 2);
	path.arcTo(rect, 0, 360);
	QRect newRect = rect;
	newRect.setSize(newRect.size() / 1.5);
	newRect.moveCenter(rect.center());
	path.moveTo(0, -newRect.top());
	path.arcTo(newRect, 0, 360);
	painter.setPen(Qt::NoPen);
	painter.setBrush(_colorInfo.backGroundBrush);
	painter.drawPath(path);

	//画进度环
	if (this->percent() > 0)
	{
		painter.setBrush(_colorInfo.bodyBrush);

		//painter.setBrush(_colorInfo.bodyBrush);
		if (_value != _maximum)
		{
			QPointF start, end;
			getArcStartEnd(size, start, end);
			path = QPainterPath();
			path.moveTo(start);
			path.arcTo(rect, 0, this->percent() * 360 / 100);
			QPointF newStart, newEnd;
			getArcStartEnd(size / 1.5, newStart, newEnd);
			QPointF center((end.x() + newEnd.x()) / 2, (end.y() + newEnd.y()) / 2);
			qreal radius = std::sqrt(std::pow(newEnd.x() - end.x(), 2) + std::pow(newEnd.y() - end.y(), 2)) / 2;
			path.arcTo(QRect(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius), this->percent() * 360 / 100, 180);
			path.arcTo(newRect, this->percent() * 360 / 100, -this->percent() * 360 / 100);
			path.lineTo(newStart);
			path.lineTo(start);
			path.setFillRule(Qt::WindingFill);
		}
		painter.drawPath(path);
	}

	//画中心文本
	painter.restore();
	QFont font = this->font();
	font.setPointSize(size / 5);
	font.setBold(true);
	painter.setPen(_colorInfo.foreground);
	painter.setFont(font);
	QFontMetrics fm(font);
	QString text = this->text();
	auto textRect = fm.boundingRect(QRect(), Qt::TextWordWrap, text);
	textRect.moveCenter(this->rect().center());
	painter.drawText(textRect, Qt::AlignCenter, this->text());

	this->update();
}

QSize LRingProgressBar::sizeHint() const
{
	int size = std::min(this->rect().width(), this->rect().height());
	size -= size / 5;
	return{ size,size };
}