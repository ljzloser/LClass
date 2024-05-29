#include "LLineedit.h"

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