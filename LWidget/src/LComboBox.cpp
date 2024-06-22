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

LHostAddressCompleter::LHostAddressCompleter(QWidget* parent)
	:QCompleter(parent)
{
}

QStringList LHostAddressCompleter::splitPath(const QString& path) const
{
	LHostAddressComboBox* comboBox = qobject_cast<LHostAddressComboBox*>(this->parent());
	LHostAddressEdit* edit = comboBox->lineEdit();
	int block = edit->currentBlock();
	QStringList list;
	list << path.split(".").mid(0, block + 1).join(".");
	return list;
}

LHostAddressComboBox::LHostAddressComboBox(bool loadAllInterfaces, QWidget* parent)
	:QComboBox(parent)
{
	this->setLineEdit(_edit);
	if (loadAllInterfaces)
	{
		QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
		foreach(QNetworkInterface interface, interfaces)
		{
			QList<QNetworkAddressEntry> entries = interface.addressEntries();
			foreach(QNetworkAddressEntry entry, entries)
			{
				this->addItem(entry.ip().toString());
			}
		}
	}
	this->setCompleter(_complter);
	_complter->setModel(this->model());
	connect(this, &QComboBox::currentTextChanged, [=](const QString& text)
		{
			emit this->currentHostAddressChanged(QHostAddress(text));
		});
}

LHostAddressComboBox::LHostAddressComboBox(QWidget* parent)
	:LHostAddressComboBox(false, parent)
{
}

LHostAddressComboBox::~LHostAddressComboBox() = default;

void LHostAddressComboBox::addItem(const QHostAddress& address)
{
	if (address.isNull() || address.protocol() != QAbstractSocket::IPv4Protocol) return;
	QString ip = address.toString();
	if (this->findText(ip) == -1)
		QComboBox::addItem(ip);
}

void LHostAddressComboBox::addItem(const QString& address)
{
	this->addItem(QHostAddress(address));
}

void LHostAddressComboBox::addItems(const QHostAddressList& addresses)
{
	for (auto&& address : addresses)
		this->addItem(address);
}

void LHostAddressComboBox::addItems(const QStringList& addresses)
{
	for (auto&& address : addresses)
		this->addItem(address);
}

void LHostAddressComboBox::removeItem(const int index)
{
	QComboBox::removeItem(index);
}

void LHostAddressComboBox::insertItem(const int index, const QHostAddress& address)
{
	if (address.isNull() || address.protocol() != QAbstractSocket::IPv4Protocol) return;
	QString ip = address.toString();
	if (this->findText(ip) == -1)
		QComboBox::insertItem(index, ip);
}

void LHostAddressComboBox::insertItem(const int index, const QString& address)
{
	this->insertItem(index, QHostAddress(address));
}

void LHostAddressComboBox::insertItems(const int index, const QHostAddressList& addresses)
{
	for (int i = 0; i < addresses.size(); ++i)
		this->insertItem(index + i, addresses[i]);
}

void LHostAddressComboBox::insertItems(const int index, const QStringList& addresses)
{
	for (int i = 0; i < addresses.size(); ++i)
		this->insertItem(index + i, addresses[i]);
}

QHostAddress LHostAddressComboBox::currentHostAddress() const
{
	return QHostAddress(this->currentText());
}

void LHostAddressComboBox::setCurrentHostAddress(const QHostAddress& address)
{
	if (address.isNull() || address.protocol() != QAbstractSocket::IPv4Protocol) return;
	QComboBox::setCurrentText(address.toString());
}

int LHostAddressComboBox::findHostAddress(const QString& address)
{
	return this->findText(address);
}

int LHostAddressComboBox::findHostAddress(const QHostAddress& address)
{
	return this->findText(address.toString());
}

QHostAddress LHostAddressComboBox::itemHostAddress(int index) const
{
	return QHostAddress(this->itemText(index));
}

LHostAddressEdit* LHostAddressComboBox::lineEdit() const
{
	return _edit;
}

void LHostAddressComboBox::setLineEdit(LHostAddressEdit* edit)
{
	_edit = edit;
	QComboBox::setLineEdit(_edit);
}

QStringList LHostAddressComboBox::hostAddressStringList() const
{
	QStringList result;
	for (int i = 0; i < this->count(); ++i)
		result.append(this->itemText(i));
	return result;
}

QHostAddressList LHostAddressComboBox::hostAddressList() const
{
	QHostAddressList result;
	for (int i = 0; i < this->count(); ++i)
		result.append(QHostAddress(this->itemText(i)));
	return result;
}