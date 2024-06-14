#include "LLineedit.h"

#include <QKeyEvent>
#include <QTimer>

using namespace ljz;

LFileLineEdit::Info::Info(QFileDialog::FileMode mode, const QString& path, const QString& title,
	const QStringList& filters, const QIcon& icon)
	:mode(mode), path(path), title(title), filters(filters), icon(icon)
{
}

LFileLineEdit::LFileLineEdit(QWidget* parent)
{
	this->init();
}

LFileLineEdit::LFileLineEdit(const QString& text, QWidget* parent)
{
	this->init();
}

QAction* LFileLineEdit::action() const
{
	return this->_action;
}

QAction* LFileLineEdit::clearAction() const
{
	return this->_clearAction;
}

LFileLineEdit::Info LFileLineEdit::info() const
{
	return this->_info;
}

void LFileLineEdit::setInfo(const Info& info)
{
	this->_info = info;
	this->reAction();
}

QStringList LFileLineEdit::selectedFiles() const
{
	return this->text().split(";");
}

QString LFileLineEdit::selectedFiles(const QString& sep) const
{
	return this->selectedFiles().join(sep);
}

void LFileLineEdit::setClearActionVisible(bool visible) const
{
	this->_clearAction->setVisible(visible);
}

bool LFileLineEdit::clearActionVisible() const
{
	return this->_clearAction->isVisible();
}

void LFileLineEdit::showFileDialog()
{
	QFileDialog* fileDialog = new QFileDialog(nullptr, this->_info.title, this->_info.path);
	fileDialog->setNameFilters(_info.filters);
	fileDialog->setFileMode(this->_info.mode);
	if (fileDialog->exec() == QDialog::Accepted)
	{
		QStringList list = fileDialog->selectedFiles();
		QString filter = fileDialog->selectedNameFilter();
		if (!list.isEmpty())
		{
			this->setText(list.join(";"));
			this->setToolTip(list.join("\n"));
			emit this->fileSelected(list, filter);
		}
	}
}

void LFileLineEdit::reAction()
{
	delete this->_action;
	this->_action = new QAction();
	this->_action->setIcon(_info.icon);
	this->setPlaceholderText(_info.title);
	this->_action->setToolTip(_info.title);
	this->addAction(this->_action, QLineEdit::TrailingPosition);
	connect(this->_action, &QAction::triggered, this, &LFileLineEdit::showFileDialog);
}

void LFileLineEdit::init()
{
	this->setReadOnly(true);
	this->_clearAction->setIcon(QIcon(":/res/res/clear.png"));
	this->_clearAction->setToolTip("清空");
	this->addAction(this->_clearAction, QLineEdit::TrailingPosition);
	connect(this->_clearAction, &QAction::triggered, [=]()
		{
			this->clear();
			this->setToolTip("");
			emit this->fileCleared();
		});
	reAction();
}

LFocusSelectLineEdit::LFocusSelectLineEdit(QWidget* parent)
	:QLineEdit(parent)
{
}

LFocusSelectLineEdit::LFocusSelectLineEdit(const QString& text, QWidget* parent)
	:QLineEdit(text, parent)
{
}

void LFocusSelectLineEdit::focusInEvent(QFocusEvent* event)
{
	QLineEdit::focusInEvent(event);
	QTimer::singleShot(100, this, SLOT(selectAll()));
}

LHostAddressValidator::LHostAddressValidator(QObject* parent)
	:QValidator(parent)
{
}

LHostAddressValidator::~LHostAddressValidator() = default;

QValidator::State LHostAddressValidator::validate(QString& input, int& pos) const
{
	QRegularExpression regularExpression("^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])?\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])?$");

	QRegularExpressionMatch match = regularExpression.match(input);
	if (match.hasMatch())
	{
		return QValidator::State::Acceptable;
	}
	return QValidator::State::Invalid;
}

LHostAddressLineEdit::LHostAddressLineEdit(const QHostAddress& hostAddress, QWidget* parent)
	:QLineEdit(parent)
{
	this->setValidator(new LHostAddressValidator(this));
	connect(this, &LHostAddressLineEdit::cursorPositionChanged, this, &LHostAddressLineEdit::handleCursorPositionChanged);
	connect(this, &LHostAddressLineEdit::selectionChanged, [=]()
		{
			if (this->selectedText() == ".")
				this->setSelection(this->cursorPosition(), 0);
		});
	this->setHostAddress(hostAddress);
	connect(this, &QLineEdit::returnPressed, this, &LHostAddressLineEdit::newHostAddress);
	connect(this, &QLineEdit::editingFinished, this, &LHostAddressLineEdit::newHostAddress);
}

LHostAddressLineEdit::LHostAddressLineEdit(const QString& text, QWidget* parent)
	:LHostAddressLineEdit(QHostAddress(text), parent)
{
}

LHostAddressLineEdit::LHostAddressLineEdit(QWidget* parent)
	:LHostAddressLineEdit(QHostAddress(), parent)
{
}

void LHostAddressLineEdit::setHostAddress(const QHostAddress& hostAddress)
{
	if (hostAddress.isNull() || hostAddress.toString() == _hostAddress.toString())
	{
		QLineEdit::setText(_hostAddress.toString());
		return;
	}
	_hostAddress = hostAddress;
	QLineEdit::setText(_hostAddress.toString());
	emit this->currentHostAddressChanged(_hostAddress);
}

QHostAddress LHostAddressLineEdit::hostAddress() const
{
	return _hostAddress;
}

int LHostAddressLineEdit::currentBlock() const
{
	return _currentblock;
}

int LHostAddressLineEdit::defaultBlock() const
{
	return _defaultblock;
}

void LHostAddressLineEdit::setCurrentBlock(const int block)
{
	if (block < 0 || block > 3)
		return;
	QStringList ipv4 = (this->text().split("."));
	if (ipv4[_currentblock].length() == 0)
	{
		ipv4[_currentblock] = _hostAddress.toString().split(".").at(_currentblock);
		QLineEdit::setText(ipv4.join("."));
	}
	_currentblock = block;
	ipv4 = (this->text().split("."));
	int beginPos = currentBlock();
	for (int i = 0; i < _currentblock; ++i)
	{
		beginPos += ipv4[i].length();
	}
	this->setSelection(beginPos, ipv4[block].length());
	emit this->currentBlockChanged(_currentblock);
}

void LHostAddressLineEdit::setDefaultBlock(const int block)
{
	if (block >= 0 && block < 4)
		_defaultblock = block;
}

void LHostAddressLineEdit::setText(const QString& text)
{
	QHostAddress hostAddress(text);
	this->setHostAddress(hostAddress);
}

void LHostAddressLineEdit::newHostAddress()
{
	this->setHostAddress(QHostAddress(this->text()));
}

void LHostAddressLineEdit::focusOutEvent(QFocusEvent* event)
{
	QLineEdit::focusOutEvent(event);
	newHostAddress();
}

void LHostAddressLineEdit::focusInEvent(QFocusEvent* event)
{
	QLineEdit::focusInEvent(event);
	QTimer::singleShot(100, [=]() {this->setCurrentBlock(_defaultblock); });
}

void LHostAddressLineEdit::keyPressEvent(QKeyEvent* event)
{
	if (this->hasSelectedText())
	{
		if (event->key() == Qt::Key_Left)
		{
			this->setCurrentBlock(_currentblock - 1);
			return;
		}
		else if (event->key() == Qt::Key_Right)
		{
			this->setCurrentBlock(_currentblock + 1);
			return;
		}
	}
	QLineEdit::keyPressEvent(event);
	if (event->key() == Qt::Key_Period)
	{
		this->nextBlock();
	}
}

void LHostAddressLineEdit::handleCursorPositionChanged(int oldPos, int newPos)
{
	if (this->hasSelectedText() && this->selectedText() == this->text())
		return;
	QStringList ipv4 = (this->text().split("."));

	int pos = newPos;
	int newBlock;
	// 判断当前光标位置是否在第几个里面
	if (pos <= ipv4[0].length())
		newBlock = 0;
	else if (pos <= ipv4[1].length() + ipv4[0].length() + 1)
		newBlock = 1;
	else if (pos <= ipv4[2].length() + ipv4[1].length() + ipv4[0].length() + 2)
		newBlock = 2;
	else if (pos <= ipv4[3].length() + ipv4[2].length() + ipv4[1].length() + ipv4[0].length() + 3)
		newBlock = 3;
	else
		newBlock = 0;
	if (newBlock != _currentblock)
		this->setCurrentBlock(newBlock);
}

void LHostAddressLineEdit::nextBlock()
{
	this->setCurrentBlock(_currentblock + 1);
}