#include "LComboBox.h"
using namespace ljz;
bool KeyPressEater::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		auto keyEvent = dynamic_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Space)
		{
			const auto listView = qobject_cast<QListView*>(watched);
			if (listView != nullptr)
			{
				const int index = listView->currentIndex().row();
				if (index != -1)
					emit activated(index);
			}
		}
		else if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)
			return QObject::eventFilter(watched, event);
		return true;
	}
	else
		return QObject::eventFilter(watched, event);
}

LMultiComboBox::LMultiComboBox(QWidget* parent)
	: QComboBox(parent)
{
	this->_lineEdit->setReadOnly(true);
	this->setLineEdit(this->_lineEdit);

	auto keyPressEater = new KeyPressEater(this);
	this->_listView->installEventFilter(keyPressEater);
	this->setView(this->_listView);
	this->QComboBox::setModel(&this->_model);
	connect(this, &QComboBox::activated, this, &LMultiComboBox::selectActivated);
	connect(keyPressEater, &KeyPressEater::activated, this, &LMultiComboBox::selectActivated);
}

void LMultiComboBox::addItem(const QString& text, const bool isCheck, const QVariant& data)
{
	auto item = new QStandardItem(text);
	item->setCheckable(true);
	item->setCheckState(isCheck ? Qt::Checked : Qt::Unchecked);
	item->setData(data, Qt::UserRole + 1);
	this->_model.appendRow(item);
	this->updateText();
}

void LMultiComboBox::addItems(const QList<ItemInfo>& items)
{
	for (auto&& item : items)
		this->addItem(item.text, item.isCheck, item.data);
}

void LMultiComboBox::addItems(const QMap<QString, bool>& items)
{
	for (auto it = items.begin(); it != items.end(); ++it)
		this->addItem(it.key(), it.value());
}

void LMultiComboBox::addItems(const QStringList& items)
{
	for (auto&& item : items)
		this->addItem(item);
}

void LMultiComboBox::removeItem(const int index)
{
	this->_model.removeRow(index);
	this->updateText();
}

void LMultiComboBox::clear()
{
	this->_model.clear();
	this->updateText();
}

QStringList LMultiComboBox::selectedItemsText() const
{
	QStringList list;
	const QString text = this->_lineEdit->text();
	if (!text.isEmpty())
		list.append(text);
	else
		list = text.split(",");
	return list;
}

QList<LMultiComboBox::ItemInfo> LMultiComboBox::selectedItems() const
{
	QList<ItemInfo> list;
	for (int i = 0; i < this->_model.rowCount(); ++i)
	{
		const auto item = this->_model.item(i);
		if (item->checkState() == Qt::Checked)
			list.append(ItemInfo(i, item->text(), item->data(Qt::UserRole + 1), item->checkState() == Qt::Checked));
	}
	return list;
}

QString LMultiComboBox::itemText(const int index) const
{
	if (index >= 0 && index < this->_model.rowCount())
		return this->_model.item(index)->text();
	else
		return "";
}

LMultiComboBox::ItemInfo LMultiComboBox::itemInfo(const int index) const
{
	ItemInfo info;
	if (index >= 0 && index < this->_model.rowCount())
	{
		const auto item = this->_model.item(index);
		info.index = index;
		info.text = item->text();
		info.data = item->data(Qt::UserRole + 1);
		info.isCheck = item->checkState() == Qt::Checked;
	}
	return info;
}

int LMultiComboBox::findItem(const QVariant& data) const
{
	for (int i = 0; i < this->_model.rowCount(); ++i)
	{
		const auto item = this->_model.item(i);
		if (item->data(Qt::UserRole + 1) == data)
			return i;
	}
	return -1;
}

void LMultiComboBox::showPopup()
{
	emit showedPopup();
	QComboBox::showPopup();
}

void LMultiComboBox::hidePopup()
{
	int width = this->view()->width(), height = this->view()->height();
	int x = QCursor::pos().x() - mapToGlobal(this->geometry().topLeft()).x() + this->geometry().x();
	int y = QCursor::pos().y() - mapToGlobal(this->geometry().topLeft()).y() + this->geometry().y();
	QRect rect(0, this->height(), width, height);
	if (!rect.contains(x, y))
	{
		emit hidedPopup();
		QComboBox::hidePopup();
	}
}

void LMultiComboBox::mousePressEvent(QMouseEvent* event)
{
	QComboBox::mousePressEvent(event);
	event->accept();
}

void LMultiComboBox::mouseReleaseEvent(QMouseEvent* event)
{
	QComboBox::mouseReleaseEvent(event);
	event->accept();
}

void LMultiComboBox::mouseMoveEvent(QMouseEvent* event)
{
	QComboBox::mouseMoveEvent(event);
	event->accept();
}

void LMultiComboBox::updateText() const
{
	QStringList list;
	for (int i = 0; i < this->_model.rowCount(); ++i)
	{
		const auto item = this->_model.item(i);
		if (item->checkState() == Qt::Checked)
			list.append(item->text());
	}
	this->_lineEdit->setText(list.join(","));
	this->_lineEdit->setToolTip(list.join("\n"));
}

void LMultiComboBox::selectActivated(int index)
{
	auto item = this->_model.item(index);
	item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
	this->updateText();
	this->itemStateChanged();
}

LCompleteComboBox::LCompleteComboBox(QWidget* parent)
	:QComboBox(parent)
{
	this->setLineEdit(new LFocusSelectLineEdit());
	this->_completer = new QCompleter();
	this->setEditable(true);
	this->setCompleter(this->_completer);
	this->_completer->setFilterMode(Qt::MatchContains);
	this->_completer->setCaseSensitivity(Qt::CaseInsensitive);
	this->_completer->setModel(this->model());
}

void LCompleteComboBox::setFilterMode(Qt::MatchFlags mode) const
{
	this->_completer->setFilterMode(mode);
}

Qt::MatchFlags LCompleteComboBox::filterMode() const
{
	return this->_completer->filterMode();
}

void LCompleteComboBox::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity) const
{
	this->_completer->setCaseSensitivity(caseSensitivity);
}

Qt::CaseSensitivity LCompleteComboBox::caseSensitivity() const
{
	return this->_completer->caseSensitivity();
}

void LCompleteComboBox::paintEvent(QPaintEvent* e)
{
	QComboBox::paintEvent(e);
}