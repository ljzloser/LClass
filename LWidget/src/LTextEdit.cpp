﻿#include "LTextEdit.h"
#include <QTextLine>
#include <QTextDocumentFragment>
#include <QScrollBar>
#include <QMessageBox>
using namespace ljz;

bool LTextEditKeyPress::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		auto keyEvent = dynamic_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Tab)
		{
			LTextEdit* textEdit = qobject_cast<LTextEdit*>(watched);
			if (textEdit != nullptr)
			{
				if (textEdit->searchToolWidget()->isHidden() || textEdit->hasFocus())
					return QObject::eventFilter(watched, event);
				else
					textEdit->searchToolWidget()->eventFilter(textEdit->searchToolWidget(), event);
			}
		}
		else
			return QObject::eventFilter(watched, event);
		return true;
	}
	else
		return QObject::eventFilter(watched, event);
}

LTextEditToolWidget::LTextEditToolWidget(QFrame* parent)
	:QFrame(parent)
{
	QVBoxLayout* layout = new QVBoxLayout();
	this->setFocusPolicy(Qt::StrongFocus);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	this->setFrameShape(QFrame::WinPanel);

	_searchLineEdit->setPlaceholderText("查找...");

	_comboBox->addItems({ "向下查找", "向上查找" });

	_searchButton->setText("查找");

	QHBoxLayout* rowLayout1 = new QHBoxLayout();
	rowLayout1->addWidget(_searchLineEdit);
	rowLayout1->addWidget(_comboBox);
	rowLayout1->addWidget(_searchButton);

	QHBoxLayout* rowLayout3 = new QHBoxLayout();
	// 区分大小写
	QLabel* caseSensitiveLabel = new QLabel(this);
	caseSensitiveLabel->setText("大小写");

	// 全字匹配
	QLabel* wholeWordLabel = new QLabel(this);
	wholeWordLabel->setText("全字");
	// 使用正则表达式
	QLabel* regularExpressionLabel = new QLabel(this);
	regularExpressionLabel->setText("正则");

	rowLayout3->addWidget(caseSensitiveLabel);
	rowLayout3->addWidget(this->_caseSensitiveButton);
	rowLayout3->addWidget(wholeWordLabel);
	rowLayout3->addWidget(this->_wholeWordButton);
	rowLayout3->addWidget(regularExpressionLabel);
	rowLayout3->addWidget(this->_regExpButton);

	auto rowLayout2 = new QHBoxLayout();

	_replaceLineEdit->setPlaceholderText("替换...");

	rowLayout2->addWidget(_replaceLineEdit);
	rowLayout2->addWidget(_replaceNextButton);
	rowLayout2->addWidget(_replaceAllButton);

	layout->addLayout(rowLayout1);
	layout->addLayout(rowLayout2);
	layout->addLayout(rowLayout3);

	QHBoxLayout* mainLayout = new QHBoxLayout();
	// 切换按钮
	QVBoxLayout* leftLayout = new QVBoxLayout();

	_button->setToolTip("在查找和替换模式之间切换");
	_button->setCheckable(true);
	_button->setIcon(QIcon(":/res/res/down.png"));

	_button->setFixedSize(20, 20);
	leftLayout->addWidget(_button);
	leftLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));
	mainLayout->addLayout(leftLayout);
	mainLayout->addLayout(layout);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(mainLayout);
	this->setFixedSize(300, 50);
	NoNewlineValidator* validator = new NoNewlineValidator();
	_searchLineEdit->setValidator(validator);
	NoNewlineValidator* validator2 = new NoNewlineValidator();
	_replaceLineEdit->setValidator(validator2);
	_replaceLineEdit->hide();
	_replaceNextButton->hide();
	_replaceAllButton->hide();
	connect(_button, &QPushButton::toggled, [this](bool checked)
		{
			if (checked)
			{
				_button->setIcon(QIcon(":/res/res/up.png"));
				_replaceLineEdit->show();
				_replaceNextButton->show();
				_replaceAllButton->show();
				this->setFixedHeight(75);
			}
			else
			{
				_button->setIcon(QIcon(":/res/res/down.png"));
				_replaceLineEdit->hide();
				_replaceNextButton->hide();
				_replaceAllButton->hide();
				this->setFixedHeight(50);
			}
		});

	connect(_searchLineEdit, &QLineEdit::textChanged, [=]()
		{
			emit this->search(_searchLineEdit->text(), createFlags(), _regExpButton->isChecked());
		});
	connect(_searchLineEdit, &QLineEdit::returnPressed, [=]()
		{
			emit this->searchMove(_searchLineEdit->text(), createFlags(), _regExpButton->isChecked());
		});
	connect(_searchButton, &QPushButton::clicked, [=]()
		{
			emit this->searchMove(_searchLineEdit->text(), createFlags(), _regExpButton->isChecked());
		});
	connect(this->_regExpButton, &LSwitchButton::stateChanged, [=]()
		{
			emit this->search(_searchLineEdit->text(), createFlags(), _regExpButton->isChecked());
		});
	connect(this->_caseSensitiveButton, &LSwitchButton::stateChanged, [=]()
		{
			emit this->search(_searchLineEdit->text(), createFlags(), _regExpButton->isChecked());
		});
	connect(this->_wholeWordButton, &LSwitchButton::stateChanged, [=]()
		{
			emit this->search(_searchLineEdit->text(), createFlags(), _regExpButton->isChecked());
		});
	connect(_replaceNextButton, &QPushButton::clicked, [=]()
		{
			emit this->replace(_searchLineEdit->text(), _replaceLineEdit->text(), createFlags(), _regExpButton->isChecked());
		});
	connect(_replaceAllButton, &QPushButton::clicked, [=]()
		{
			emit this->replaceAll(_searchLineEdit->text(), _replaceLineEdit->text(), createFlags(), _regExpButton->isChecked());
		});

	// LTextEditToolWidget类 的背景色为黑色半透明
}

void LTextEditToolWidget::setSearchText(const QString& text)
{
	this->_searchLineEdit->setText(text);
	emit this->search(_searchLineEdit->text(), createFlags(), _regExpButton->isChecked());
}

void LTextEditToolWidget::init()
{
	_replaceLineEdit->clear();
	_searchLineEdit->clear();
	_button->setChecked(false);
	_button->setIcon(QIcon(":/res/res/down.png"));
	_replaceLineEdit->hide();
	_replaceNextButton->hide();
	_replaceAllButton->hide();
	this->setFixedHeight(50);
}

void LTextEditToolWidget::focusOutEvent(QFocusEvent* event)
{
	QWidget::focusOutEvent(event);
	emit this->search(_searchLineEdit->text(), createFlags());
}

QTextDocument::FindFlags LTextEditToolWidget::createFlags() const
{
	QTextDocument::FindFlags findFlags;
	if (_comboBox->currentIndex() == 0)
		findFlags &= ~QTextDocument::FindBackward;
	else
		findFlags |= QTextDocument::FindBackward;
	if (_caseSensitiveButton->state() == LSwitchButton::State::On)
		findFlags |= QTextDocument::FindCaseSensitively;
	else
		findFlags &= ~QTextDocument::FindCaseSensitively;
	if (_wholeWordButton->state() == LSwitchButton::State::On)
		findFlags |= QTextDocument::FindWholeWords;
	else
		findFlags &= ~QTextDocument::FindWholeWords;
	return findFlags;
}

void LTextEditToolWidget::focusInEvent(QFocusEvent* event)
{
	QFrame::focusInEvent(event);
	emit this->search(_searchLineEdit->text(), createFlags());
}

void LTextEditToolWidget::keyReleaseEvent(QKeyEvent* event)
{
}

LTextEdit::LTextEdit(QWidget* parent)
	:QTextEdit(parent)
{
	this->_searchToolWidget->hide();
	auto filter = new LTextEditKeyPress(this);
	this->installEventFilter(filter);
	connect(this->_searchToolWidget, &LTextEditToolWidget::search, this, &LTextEdit::search);
	connect(this->_searchToolWidget, &LTextEditToolWidget::searchMove, this, &LTextEdit::searchMove);
	connect(this->_searchToolWidget, &LTextEditToolWidget::replace, this, &LTextEdit::replace);
	connect(this->_searchToolWidget, &LTextEditToolWidget::replaceAll, this, &LTextEdit::replaceAll);
	connect(this, &QTextEdit::textChanged, this, &LTextEdit::textChangedSlot);
	_highlighter->setSelectBrush(_colorInfo.rangeFindBrush);
	_highlighter->setSearchBrush(_colorInfo.allFindBrush);
	_highlighter->setNextBrush(_colorInfo.nextFindBrush);
}

void LTextEdit::copySelectCursor(QTextCursor cursor)
{
	if (cursor.hasSelection() && _selectCursor.isNull())
	{
		_selectCursor = QTextCursor(this->textCursor());
		_highlighter->setSelectCursor(_selectCursor);
	}

	if (!_selectCursor.isNull())
	{
		QTextCursor tmpCursor = _selectCursor;
		tmpCursor.clearSelection();
		this->setTextCursor(tmpCursor);
	}
	_highlighter->rehighlight();
}

void LTextEdit::search(const QString& text, QTextDocument::FindFlags findFlags, bool isRegExp)
{
	/*
	这🀄️方式可以使用想·
	this->blockSignals(true); 来实现阻塞信号，从而达到下面的格式变化不加入到撤销栈中的效果，但是效果不好
	QTextCursor cursor = this->textCursor();
	int from = 0;
	int to = this->toPlainText().length();

	cursor.movePosition(QTextCursor::Start);
	QTextCharFormat fmt;
	fmt.setForeground(Qt::black);

	cursor.setPosition(0, QTextCursor::MoveAnchor);
	cursor.setPosition(this->toPlainText().length(), QTextCursor::KeepAnchor);
	cursor.setCharFormat(fmt);

	this->copySelectCursor(this->textCursor());

	QTextDocument* document = this->document();

	int begin = QTextDocument::FindBackward & findFlags ? to : from;
	if (text.isEmpty())
		return;
	if (isRegExp)
		cursor = document->find(QRegularExpression(text), begin, findFlags);
	else
		cursor = document->find(text, begin, findFlags);
	fmt.setForeground(_colorInfo.allFindBrush);
	int pos = cursor.position();
	while (!cursor.isNull())
	{
		if (cursor.position() <= to && cursor.position() >= from)
		{
			cursor.mergeCharFormat(fmt);
			if (!isRegExp)
				cursor = document->find(text, cursor, findFlags);
			else
				cursor = document->find(QRegularExpression(text), cursor, findFlags);
			if (cursor.position() == pos)
				break;
			else
				pos = cursor.position();
		}
	}
	*/
	_isRegExp = isRegExp;
	_findFlags = findFlags;
	_lastCursor = QTextCursor();
	_findData = text;
	disconnect(this, &QTextEdit::textChanged, this, &LTextEdit::textChangedSlot);
	this->blockSignals(true);
	this->setUpdatesEnabled(false);
	QRegularExpression re(text);

	if (!isRegExp)
	{
		re = QRegularExpression(LFunc::escapeSpecialCharacters(re.pattern()));
		// 是否全字匹配
		if (findFlags & QTextDocument::FindWholeWords)
		{
			re = QRegularExpression("\\b" + re.pattern() + "\\b");
		}
	}
	// 是否区分大小写
	if (!(findFlags & QTextDocument::FindCaseSensitively))
	{
		re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	}
	_highlighter->setRe(re);
	this->copySelectCursor(this->textCursor());
	this->blockSignals(false);
	this->setUpdatesEnabled(true);
	connect(this, &QTextEdit::textChanged, this, &LTextEdit::textChangedSlot);
}

void LTextEdit::searchMove(const QString& text, QTextDocument::FindFlags findFlags, bool isRegExp)
{
	if (text.isEmpty())
		return;
	disconnect(this, &QTextEdit::textChanged, this, &LTextEdit::textChangedSlot);
	QTextCursor cursor = this->textCursor();
	QTextDocument* document = this->document();
	document->setUndoRedoEnabled(false);
	QTextCursor findCursor;
	if (!isRegExp)
		findCursor = document->find(text, cursor, findFlags);
	else
		findCursor = document->find(QRegularExpression(text), cursor, findFlags);
	if (findFlags & QTextDocument::FindBackward && !_lastCursor.isNull())
	{
		if (!isRegExp)
			findCursor = document->find(text, findCursor, findFlags);
		else
		{
			QRegularExpression re(text);
			// 是否区分大小写
			if (!(findFlags & QTextDocument::FindCaseSensitively))
			{
				re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			}
			findCursor = document->find(re, findCursor, findFlags);
		}
	}

	this->copySelectCursor(this->textCursor());
	if (!_selectCursor.isNull())
	{
		int a = _selectCursor.selectionStart();
		int b = _selectCursor.selectionEnd();
		int from = a < b ? a : b;
		int to = a < b ? b : a;
		int pos = findCursor.position();
		if ((pos < from || pos > to) || findCursor.isNull())
		{
			emit this->notFind();
			connect(this, &QTextEdit::textChanged, this, &LTextEdit::textChangedSlot);
			return;
		}
	}
	_lastCursor = findCursor;
	_highlighter->setNextCursor(findCursor);
	_highlighter->rehighlight();

	findCursor.clearSelection();
	if (!findCursor.isNull())
	{
		this->setTextCursor(findCursor);
	}

	connect(this, &QTextEdit::textChanged, this, &LTextEdit::textChangedSlot);
}

void LTextEdit::replace(const QString& text, const QString& replaceText, QTextDocument::FindFlags findFlags, bool isRegExp)
{
	if (text.isEmpty())
		return;
	QTextCursor cursor = this->textCursor();
	QTextDocument* document = this->document();
	QTextCursor findCursor;

	QRegularExpression re(text);

	if (!isRegExp)
		findCursor = document->find(text, _lastCursor.isNull() ? cursor : _lastCursor, findFlags);
	else
		findCursor = document->find(re, _lastCursor.isNull() ? cursor : _lastCursor, findFlags);

	this->copySelectCursor(this->textCursor());
	if (!_selectCursor.isNull())

	{
		int a = _selectCursor.selectionStart();
		int b = _selectCursor.selectionEnd();
		int from = a < b ? a : b;
		int to = a < b ? b : a;
		int pos = findCursor.position();
		if ((pos < from || pos > to) || findCursor.isNull())
		{
			emit this->notFind();
			return;
		}
	}
	_lastCursor = findCursor;

	if (isRegExp)
	{
		QString findText = findCursor.selectedText();
		QString newReplaceText = QString(replaceText);
		QRegularExpression expression(text);
		if (!(findFlags & QTextDocument::FindCaseSensitively))
		{
			expression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
		}
		QRegularExpressionMatch match = expression.match(findText);
		// 判断replaceText中是否有 $1 $2 ...
		QRegularExpression replaceRe("\\$([0-9]+)");
		QRegularExpressionMatchIterator it = replaceRe.globalMatch(replaceText);
		while (it.hasNext())
		{
			QRegularExpressionMatch thisMatch = it.next();
			QString number = thisMatch.captured(1);
			QString newString = match.captured(number.toInt());
			newReplaceText.replace("$" + number, newString);
		}
		findCursor.insertText(newReplaceText);
	}
	else
	{
		findCursor.insertText(replaceText);
	}
	if (!findCursor.isNull())
	{
		this->setTextCursor(findCursor);
	}
}

void LTextEdit::replaceAll(const QString& text, const QString& replaceText, QTextDocument::FindFlags findFlags,
	bool isRegExp)
{
	if (text.isEmpty())
		return;
	disconnect(this, &QTextEdit::textChanged, this, &LTextEdit::textChangedSlot);
	this->setUpdatesEnabled(false);
	int from = 0;
	int to;
	this->copySelectCursor(this->textCursor());
	int count = 0;
	if (!_selectCursor.isNull())
	{
		int a = _selectCursor.selectionStart();
		int b = _selectCursor.selectionEnd();
		from = a < b ? a : b;
	}
	QRegularExpression re(text);
	QTextCursor findCursor(document());
	// 将光标移动到from位置
	findCursor.setPosition(from, QTextCursor::MoveAnchor);

	findCursor.beginEditBlock(); // 将所有的替换操作合并在一起放入撤销栈中
	do
	{
		from = 0;
		to = this->document()->toPlainText().length();
		if (!_selectCursor.isNull())
		{
			int a = _selectCursor.selectionStart();
			int b = _selectCursor.selectionEnd();
			from = a < b ? a : b;
			to = a < b ? b : a;
		}
		QTextCursor tmpCursor;
		if (!isRegExp)
			tmpCursor = this->document()->find(text, findCursor, findFlags);
		else
			tmpCursor = this->document()->find(re, findCursor, findFlags);

		int pos = tmpCursor.position();

		if ((pos < from || pos > to) || tmpCursor.isNull())
		{
			emit this->notFind();
			break;
		}
		else
		{
			// 因为要合并到一起，所以findCursor不能通过 = 来赋值，而是通过 setPosition 来模拟移动光标
			findCursor.setPosition(tmpCursor.selectionStart(), QTextCursor::MoveAnchor);
			findCursor.setPosition(tmpCursor.selectionEnd(), QTextCursor::KeepAnchor);
			if (isRegExp)
			{
				QString findText = findCursor.selectedText();
				QString newReplaceText = QString(replaceText);
				QRegularExpression regularExpression(text);
				if (!(findFlags & QTextDocument::FindCaseSensitively))
				{
					regularExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
				}
				QRegularExpressionMatch match = regularExpression.match(findText);
				// 判断replaceText中是否有 11 1 21 1 31 1 ...
				QRegularExpression replaceRe("\\$([0-9]+)");
				QRegularExpressionMatchIterator it = replaceRe.globalMatch(replaceText);
				while (it.hasNext())
				{
					QRegularExpressionMatch thisMatch = it.next();
					QString number = thisMatch.captured(1);
					QString newString = match.captured(number.toInt());
					newReplaceText.replace("$" + number, newString);
				}
				findCursor.insertText(newReplaceText);
			}
			else
			{
				findCursor.insertText(replaceText);
			}
			count++;
			this->setTextCursor(findCursor);
		}
	} while (!findCursor.isNull());
	findCursor.endEditBlock();

	this->setUpdatesEnabled(true);
	connect(this, &QTextEdit::textChanged, this, &LTextEdit::textChangedSlot);
	_highlighter->rehighlight();
	this->update();
	QMessageBox::information(this, "提示", QString("共替换了 %1 个").arg(count));
}

void LTextEdit::keyPressEvent(QKeyEvent* event)
{
	QTextEdit::keyPressEvent(event);
	if (event->key() == Qt::Key_F && (event->modifiers() & Qt::ControlModifier))
	{
		if (this->_searchToolWidget->isHidden())
		{
			this->_searchToolWidget->init();
			this->_searchToolWidget->show();
			this->_searchToolWidget->setFocus();
			QString text = _selectCursor.isNull() ? this->textCursor().selectedText() : _selectCursor.selectedText();
			bool isLines = text.contains("\xE2\x80\xA9""");
			if (!isLines)
				this->_searchToolWidget->setSearchText(text);
			this->copySelectCursor(this->textCursor());
			_highlighter->setIsHighlight(true);
			_highlighter->rehighlight();
		}
		else
		{
			this->_searchToolWidget->hide();
			this->setFocus();
			_highlighter->setIsHighlight(false);
			_highlighter->rehighlight();
			_selectCursor = QTextCursor();
		}
		QRect rect = this->_searchToolWidget->rect();
		int right = this->verticalScrollBar()->isHidden() ? 0 : this->verticalScrollBar()->width();
		rect.moveTopRight(this->rect().topRight());
		rect.moveRight(rect.right() - right - 1);
		this->_searchToolWidget->setGeometry(rect);
	}
}

void LTextEdit::resizeEvent(QResizeEvent* event)
{
	QTextEdit::resizeEvent(event);
	QRect rect = this->_searchToolWidget->rect();
	int right = this->verticalScrollBar()->isHidden() ? 0 : this->verticalScrollBar()->width();
	rect.moveTopRight(this->rect().topRight());
	rect.moveRight(rect.right() - right - 1);
	this->_searchToolWidget->setGeometry(rect);
}

void LTextEdit::focusInEvent(QFocusEvent* event)
{
	QTextEdit::focusInEvent(event);
	_selectCursor = QTextCursor();
	_highlighter->setSelectCursor(_selectCursor);
	this->search(_findData.toString(), _findFlags, _isRegExp);
}

void LTextEdit::focusOutEvent(QFocusEvent* event)
{
	QTextEdit::focusOutEvent(event);
	_selectCursor = QTextCursor();
	_highlighter->setSelectCursor(_selectCursor);
	this->search(_findData.toString(), _findFlags, _isRegExp);
}

void LTextEdit::textChangedSlot()
{
	QString hashText = QCryptographicHash::hash(this->toPlainText().toUtf8(), QCryptographicHash::Sha256).toHex();
	if (this->_searchToolWidget && !this->_searchToolWidget->isHidden())
	{
		if (hashText != _hashText)
		{
			this->search(this->_findData.toString(), this->_findFlags, this->_isRegExp);
		}
	}
	_hashText = hashText;
}
void LTextEdit::paintEvent(QPaintEvent* event)
{
	QTextEdit::paintEvent(event);
}