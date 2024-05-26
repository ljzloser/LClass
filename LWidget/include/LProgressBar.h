#pragma once
#include "lwidget_global.h"
#include "QWidget"
#include <QPropertyAnimation>
namespace ljz
{
	class LWIDGET_EXPORT LBaseProgressBar : public QWidget
	{
		Q_OBJECT
			Q_PROPERTY(qint32 minimum READ minimum WRITE setMinimum)
			Q_PROPERTY(qint32 maximum READ maximum WRITE setMaximum)
			Q_PROPERTY(QString text READ text)
			Q_PROPERTY(qint32 value READ value WRITE setValue NOTIFY valueChanged)
	public:
		[[nodiscard]] virtual int minimum() const;
		[[nodiscard]] virtual int maximum() const;
		[[nodiscard]] virtual int value() const;
		[[nodiscard]] virtual QString format() const;
		[[nodiscard]] virtual QString text() const;
		[[nodiscard]] virtual double percent() const;

		explicit LBaseProgressBar(QWidget* parent = nullptr)
			: QWidget(parent)
		{}
		explicit LBaseProgressBar(const int minimum, const int maximum, QWidget* parent = nullptr)
			: QWidget(parent), _minimum(minimum), _maximum(maximum)
		{}
		explicit LBaseProgressBar(const LBaseProgressBar* other, QWidget* parent = nullptr)
			:QWidget(parent), _minimum(other->minimum()), _maximum(other->maximum()),
			_value(other->value()), _format(other->format())
		{}
		~LBaseProgressBar() override = default;
	public slots:
		virtual void setMinimum(const int minimum);
		virtual void setMaximum(const int maximum);
		virtual void setRange(const int minimum, const int maximum);
		virtual void setValue(const int value);

		/**
		 * %v: value, %p: percent, %m: maximum\n
		 * example: setFormat("%v/%m\n%p%")
		 * @param format default: "%p%"
		 */
		virtual void setFormat(const QString& format = "%p%");
		virtual void addValue(const int offset = 1);
	signals:
		void valueChanged(int value);
	protected:
		void paintEvent(QPaintEvent* event) override = 0;
		QSize sizeHint() const override = 0;
		int _minimum{ 0 };
		int _maximum{ 0 };
		int _value{ 0 };
		QString _format{ "%p%" };
	};

	class LWIDGET_EXPORT LRingProgressBar final : public LBaseProgressBar
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
			:LBaseProgressBar(parent)
		{}
		explicit LRingProgressBar(const int minimum, const int maximum, QWidget* parent = nullptr)
			:LBaseProgressBar(minimum, maximum, parent)
		{}
		explicit LRingProgressBar(const LBaseProgressBar* other, QWidget* parent = nullptr)
			:LBaseProgressBar(other, parent)
		{}
		~LRingProgressBar() override = default;
		void setColorInfo(const ColorInfo& colorInfo) { _colorInfo = colorInfo; }
		[[nodiscard]] ColorInfo colorInfo() const { return _colorInfo; }

	protected:
		void getArcStartEnd(int size, QPointF& start, QPointF& end) const;
		void paintEvent(QPaintEvent* event) override;
		QSize sizeHint() const override;
		ColorInfo _colorInfo;
	};
}
