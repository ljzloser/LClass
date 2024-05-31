#include "LNavigationBar.h"
using namespace ljz;

void LNavigationBar::setItems(const QList<Item>& items)
{
	this->_items = items;
}

QList<LNavigationBar::Item> LNavigationBar::items()
{
	return _items;
}

void LNavigationBar::setShowCount(int count)
{
	_showCount = count;
}

int LNavigationBar::showCount() const
{
	return _showCount;
}

void LNavigationBar::setItemSize(int size)
{
	_itemSize = size;
}

int LNavigationBar::itemSize() const
{
	return _itemSize;
}

void LNavigationBar::setOrientation(Qt::Orientation orientation)
{
	_orientation = orientation;
}

Qt::Orientation LNavigationBar::orientation() const
{
	return _orientation;
}