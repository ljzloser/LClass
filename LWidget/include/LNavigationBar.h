#pragma once
#include "lwidget_global.h"
#include <QWidget>
#include <QStringListModel>
#include <QGraphicsView>
#include "LComboBox.h"
#include <QToolBar>

namespace ljz
{
	class LWIDGET_EXPORT LNavigationBar final :public QWidget
	{
		struct Item
		{
			QString title = "";
			QPixmap pixmap = QPixmap();
			Item() = default;
		};
		Q_OBJECT
	public:
		void setItems(const QList<Item>& items);
		QList<Item> items();
		void setShowCount(int count);
		int showCount() const;
		void setItemSize(int size);
		int itemSize() const;
		void setOrientation(Qt::Orientation orientation);
		Qt::Orientation orientation() const;
	signals:
		void itemClicked(int index, const Item& item);
	protected:
		QList<Item> _items;
		int _showCount = 5;
		int _itemSize = 30;
		Qt::Orientation _orientation = Qt::Vertical;
	};
}
