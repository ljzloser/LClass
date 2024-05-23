#include "LInstrumentBoard.h"

#include <QApplication>
#include <QMenu>
#include <QClipboard>
using namespace ljz;
LInstrumentBoard::LInstrumentBoard(double minValue, double maxValue, int precision, QWidget* parent)
	:QWidget(parent)
{
	this->setRange(minValue, maxValue, precision);
	this->init();
}
LInstrumentBoard::LInstrumentBoard(int minValue, int maxValue, QWidget* parent)
	:QWidget(parent)
{
	this->setRange(minValue, maxValue);
	this->init();
}
LInstrumentBoard::~LInstrumentBoard()
= default;

void LInstrumentBoard::init()
{
	this->_value = _minValue;
	this->_drawValue = _minValue;
	this->_colorMap = QMap<ColorKey, QColor>({
		{ColorKey::Background,QColor(Qt::GlobalColor::transparent)},
		{ColorKey::MaxCir,QColor(Qt::GlobalColor::darkCyan)},
		{ColorKey::MidCir,QColor(Qt::GlobalColor::lightGray)},
		{ColorKey::Range,QColor(Qt::red)},
		{ColorKey::MinCir,QColor(Qt::GlobalColor::black)},
		{ColorKey::Scale,QColor(Qt::GlobalColor::black)},
		{ColorKey::Number,QColor(Qt::GlobalColor::magenta)},
		{ColorKey::CenterCir,QColor(Qt::GlobalColor::darkCyan)},
		{ColorKey::Ptr,QColor(Qt::blue)},
		{ColorKey::Value,QColor(Qt::GlobalColor::white)},
		{ColorKey::Name,QColor(Qt::GlobalColor::lightGray)}
		});
	connect(this->_timer, &QTimer::timeout, this, &LInstrumentBoard::timeOutSlot);
	this->_timer->start(this->_timeChange);
	//    this->setWindowFlags(Qt::FramelessWindowHint);
	//    this->setAttribute(Qt::WA_TranslucentBackground, true);
}

double LInstrumentBoard::valueToAngle() const
{
	double oneAngle = (_maxValue - _minValue) / 270;
	double result = -(this->_drawValue - _maxValue) / oneAngle - 45;
	if (result < -45)
	{
		result = -45;
	}
	else if (result > 225)
	{
		result = 225;
	}
	return result;
}

double LInstrumentBoard::angleToValue(int angle) const
{
	double oneAngle = (_maxValue - _minValue) / 270;
	return _maxValue - (angle + 45) * oneAngle;
}

void LInstrumentBoard::timeOutSlot()
{
	double oneAngle = (_maxValue - _minValue) / 270 * 5;
	if (this->_drawValue < this->_value)
	{
		this->_drawValue = qMin(this->_drawValue + oneAngle, _value);
		update();
	}
	else
	{
		this->_drawValue = qMax(this->_drawValue - oneAngle, _value);
		update();
	}
}
void LInstrumentBoard::setRange(double minValue, double maxValue, int precision)
{
	this->_minValue = minValue;
	this->_maxValue = maxValue;
	this->_precision = precision;
}
void LInstrumentBoard::setRange(int minValue, int maxValue)
{
	this->_minValue = minValue;
	this->_maxValue = maxValue;
	this->_precision = 0;
}

void LInstrumentBoard::setColor(ColorKey key, QColor color)
{
	this->_colorMap[key] = color;
}

void LInstrumentBoard::setColor(QMap<ColorKey, QColor>& colorMap)
{
	this->_colorMap = colorMap;
}
double LInstrumentBoard::getMinValue() const
{
	return this->_minValue;
}
double LInstrumentBoard::getMaxValue() const
{
	return this->_maxValue;
}
int LInstrumentBoard::getPrecision() const
{
	return this->_precision;
}
void LInstrumentBoard::setValue(int value)
{
	this->setValue(static_cast<double>(value));
}
void LInstrumentBoard::setValue(double value)
{
	this->_value = value;
	if (!this->isOpenAnimation())
	{
		this->_drawValue = value;
	}
	emit this->valueChange(this->_value);
}

void LInstrumentBoard::setUnit(QString unit)
{
	this->_unit = unit;
}

void LInstrumentBoard::setName(QString Name)
{
	this->_name = Name;
}

void LInstrumentBoard::setOpenAnimation(bool isOpen)
{
	if (this->_isOpenAnimation == isOpen)
	{
		return;
	}

	this->_isOpenAnimation = isOpen;
	if (isOpen)
	{
		this->_timer->start(this->_timeChange);
	}
	else
	{
		this->_timer->stop();
		this->_drawValue = this->_value;
	}
}
double LInstrumentBoard::getValue() const
{
	return this->_value;
}

const QMap<LInstrumentBoard::ColorKey, QColor>& LInstrumentBoard::getColor()
{
	return this->_colorMap;
}

QString LInstrumentBoard::getUnit() const
{
	return this->_unit;
}

QColor LInstrumentBoard::getColor(ColorKey key) const
{
	return this->_colorMap[key];
}

void LInstrumentBoard::paintEvent(QPaintEvent* e)
{
	// 获取需要绘制的区域
	QRectF rect = e->rect();

	// 创建 QPainter 对象，并指定绘图设备为当前窗口
	QPainter painter(this);
	painter.setPen(this->_colorMap[ColorKey::Background]);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(this->_colorMap[ColorKey::Background]);

	// 绘制背景
	painter.drawRect(rect);
	QRectF boardRect = QRectF(20, 20, this->width() - 40, this->height() - 40);

	double side = qMin(boardRect.width(), boardRect.height()); // 获取较小尺寸
	// 画仪表盘外圆
	painter.setPen(this->_colorMap[ColorKey::MaxCir]);
	painter.setBrush(this->_colorMap[ColorKey::MaxCir]);
	painter.drawEllipse(boardRect.center(), side / 2, side / 2);

	// 画仪表盘中圆

	//    painter.setPen(this->_colorMap[ColorKey::MidCir]);
	//    painter.setBrush(this->_colorMap[ColorKey::MidCir]);
	//    painter.drawEllipse(boardRect.center(),int(side / 2 / 20 * 19) ,int(side / 2 / 20 * 19));

	QRectF outRect(0, 0, double(side / 20 * 19), double(side / 20 * 19));//矩形长宽为窗口的长宽
	outRect.moveCenter(boardRect.center());

	painter.setPen(Qt::NoPen);
	painter.setBrush(QBrush(QColor(this->_colorMap[ColorKey::MidCir])));
	painter.drawEllipse(outRect);

	QConicalGradient gradient;
	// 定义渐变色
	gradient.setColorAt(0, this->_colorMap[ColorKey::MinCir]);
	gradient.setColorAt(0.5, this->_colorMap[ColorKey::MidCir]);
	gradient.setColorAt(1, this->_colorMap[ColorKey::Range]);

	painter.setBrush(gradient);
	painter.drawPie(outRect, (225) * 16, static_cast<int>(225 - valueToAngle()) * -16);

	// 画仪表盘内圆

	painter.setPen(this->_colorMap[ColorKey::MinCir]);
	painter.setBrush(this->_colorMap[ColorKey::MinCir]);
	painter.drawEllipse(boardRect.center(), double(side / 2 / 20 * 15), double(side / 2 / 20 * 15));

	// 画仪表盘刻度
	QPen ScalePen = QPen(this->_colorMap[ColorKey::Scale]);
	ScalePen.setWidth(2);
	painter.setPen(ScalePen);
	QList<QPointF> points;
	for (int i = -45; i <= 225; i += 5) {
		qreal rad = qDegreesToRadians(double(i));
		int k = i % 45 == 0 ? 15 : 17;
		QPointF outerPoint((boardRect.center().x() + (side / 2 / 20 * 19) * qCos(rad)),
			(boardRect.center().y() - (side / 2 / 20 * 19) * qSin(rad)));
		QPointF innerPoint((boardRect.center().x() + (side / 2 / 20 * k) * qCos(rad)),
			(boardRect.center().y() - (side / 2 / 20 * k) * qSin(rad)));
		painter.drawLine(QLineF(outerPoint, innerPoint));
		if (k == 15)
		{
			points.append(innerPoint);
		}
		//painter.drawText(outerPoint, QString::number(i));
	}

	//    QPen pen1(QColor(this->_colorMap[ColorKey::CenterCir]));
	//    pen1.setWidth(2);
	//    painter.setPen(pen1);
	//    for (int i = 0; i < points.size(); i++ )
	//    {
	//        int begin = i;
	//        int end = i + 1 == points.size() ? 0 : i + 1;
	//        painter.drawLine(points[begin],points[end]);
	////        painter.drawLine(points[begin],boardRect.center());
	//    }

	// 绘制文字
	QFont font = QFont("Arial", static_cast<int>(side / 30));
	painter.setFont(font);
	painter.setPen(this->_colorMap[ColorKey::Number]);
	for (int i = -45; i <= 225; i += 45) {
		qreal rad = qDegreesToRadians(double(i));
		QPointF outerPoint((boardRect.center().x() + (side / 2 / 20 * 12) * qCos(rad)),
			(boardRect.center().y() - (side / 2 / 20 * 12) * qSin(rad)));
		QString text = QString("%1%2").arg(QString::number(this->angleToValue(i), 'f', this->_precision)).arg("");
		// 计算文字的大小
		QFontMetrics fm(font);
		QRectF textRect = fm.boundingRect(text);
		textRect.moveCenter(outerPoint);
		painter.drawText(textRect, Qt::AlignCenter, text); // 绘制文字
	}
	// 画指针
	QPen pen(Qt::red);
	pen.setWidth(static_cast<int>(side / 60));
	painter.setPen(pen);
	painter.setBrush(Qt::red);
	qreal rad = qDegreesToRadians(valueToAngle());
	QPointF center = boardRect.center();
	int radius = static_cast<int>(side / 2 / 20 * 10);
	QPointF hand(static_cast<qreal>(this->width() / 2) + radius * qCos(rad),
		static_cast<qreal>(this->height() / 2) - radius * qSin(rad));
	painter.drawLine(QLineF(center, hand));

	// 画仪表盘中心圆

	painter.setPen(this->_colorMap[ColorKey::CenterCir]);
	painter.setBrush(this->_colorMap[ColorKey::CenterCir]);
	painter.drawEllipse(boardRect.center(), double(side / 2 / 20 * 2), double(side / 2 / 20 * 2));

	// 画数值
	QString text = QString("%1%2").arg(QString::number(this->_drawValue, 'f', this->_precision)).arg(this->_unit);
	painter.setPen(this->_colorMap[ColorKey::Value]);
	font = QFont("Arial", static_cast<int>(side / 20));
	painter.setFont(font);
	QPointF p(boardRect.center().x(), boardRect.center().y() + (side / 2 / 20 * 9));
	// 计算文字的大小
	QFontMetrics fm(font);
	QRectF textRect = fm.boundingRect(text);
	textRect.moveCenter(p);
	painter.drawText(textRect, Qt::AlignCenter, text); // 绘制文字

	//画名称
	painter.setPen(this->_colorMap[ColorKey::Name]);
	p = QPointF(boardRect.center().x(), boardRect.center().y() + (side / 2 / 20 * 12));
	textRect = fm.boundingRect(this->_name);
	textRect.moveCenter(p);
	painter.drawText(textRect, Qt::AlignCenter, this->_name); // 绘制文字
}

void LInstrumentBoard::contextMenuEvent(QContextMenuEvent* e)
{
	QMenu menu(this);
	QAction copyAction("复制");
	menu.addAction(&copyAction);
	connect(&copyAction, &QAction::triggered, [this]()
		{
			QClipboard* clipboard = QApplication::clipboard();
			QString text = QString("{%1 : %2%3}").arg(this->getName()).arg(QString::number(this->_drawValue, 'f', this->_precision)).arg(this->_unit);
			clipboard->setText(text);
		});
	menu.exec(e->globalPos());
}