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

LFileLineEdit::LFileLineEdit(const QString& text, QWidget* parent)
	:QLineEdit(text, parent)
{
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

LHostAddressEdit::LHostAddressEdit(const QHostAddress& hostAddress, QWidget* parent)
	:QLineEdit(parent)
{
	this->installEventFilter(this);
	this->setValidator(new LHostAddressValidator(this));
	connect(this, &LHostAddressEdit::cursorPositionChanged, this, &LHostAddressEdit::handleCursorPositionChanged);
	connect(this, &LHostAddressEdit::selectionChanged, [=]()
		{
			if (this->selectedText() == ".")
				this->setSelection(this->cursorPosition(), 0);
		});
	this->setHostAddress(hostAddress);
	//connect(this, &QLineEdit::editingFinished, this, &LHostAddressEdit::hostAddressEditFinish);
}

LHostAddressEdit::LHostAddressEdit(const QString& text, QWidget* parent)
	:LHostAddressEdit(QHostAddress(text), parent)
{
}

LHostAddressEdit::LHostAddressEdit(QWidget* parent)
	:LHostAddressEdit(QHostAddress(), parent)
{
}

void LHostAddressEdit::setHostAddress(const QHostAddress& hostAddress)
{
	if (hostAddress.isNull() || hostAddress.protocol() != QAbstractSocket::IPv4Protocol || hostAddress.toString() == _hostAddress.toString())
	{
		if (this->text() != _hostAddress.toString())
			QLineEdit::setText(_hostAddress.toString());
		return;
	}
	_hostAddress = hostAddress;
	QLineEdit::setText(_hostAddress.toString());
	emit this->hostAddressChanged(_hostAddress);
}

QHostAddress LHostAddressEdit::hostAddress() const
{
	return _hostAddress;
}

int LHostAddressEdit::currentBlock() const
{
	return _currentblock;
}

int LHostAddressEdit::defaultBlock() const
{
	return _defaultblock;
}

void LHostAddressEdit::setCurrentBlock(const int block)
{
	if (block < 0 || block > 3)
		return;
	this->setHostAddress(QHostAddress(this->text()));
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
	if (this->hasFocus())
		this->setSelection(beginPos, ipv4[_currentblock].length());
	emit this->currentBlockChanged(_currentblock);
}

void LHostAddressEdit::setDefaultBlock(const int block)
{
	if (block >= 0 && block < 4)
		_defaultblock = block;
}

void LHostAddressEdit::setText(const QString& text)
{
	QHostAddress hostAddress(text);
	this->setHostAddress(hostAddress);
}

void LHostAddressEdit::hostAddressEditFinish()
{
	this->setHostAddress(QHostAddress(this->text()));
	emit this->hostAddressEditFinished(_hostAddress);
}

void LHostAddressEdit::focusOutEvent(QFocusEvent* event)
{
	QLineEdit::focusOutEvent(event);
	hostAddressEditFinish();
}

void LHostAddressEdit::focusInEvent(QFocusEvent* event)
{
	QLineEdit::focusInEvent(event);
	QTimer::singleShot(100, [=]() {this->setCurrentBlock(_defaultblock); });
}

void LHostAddressEdit::keyPressEvent(QKeyEvent* event)
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

void LHostAddressEdit::handleCursorPositionChanged(int oldPos, int newPos)
{
	if (this->hasSelectedText() && this->selectedText() == this->text())
		return;
	QStringList ipv4 = (this->text().split("."));
	if (ipv4.length() != 4) return;
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

bool LHostAddressEdit::eventFilter(QObject* watched, QEvent* event)
{
	/*
	 * 这里拦截了输入法，如果输入的是.，则调用nextBlock(),因为在centos上，无法捕获到
	 * .键，所以需要在eventFilter中捕获输入的.
	 */
	if (event->type() == QEvent::InputMethod)
	{
		QInputMethodEvent* inputEvent = static_cast<QInputMethodEvent*>(event);
		// 判断是不是输入的.
		if (inputEvent->commitString() == ".")
		{
			this->nextBlock();
			return false;
		}
	}

	return QLineEdit::eventFilter(watched, event);
}

void LHostAddressEdit::nextBlock()
{
	this->setCurrentBlock(_currentblock + 1);
}