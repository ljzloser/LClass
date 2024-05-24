#pragma once
#include <QTextEdit>
#include "lwidget_global.h"
#include  <QLayout>
#include  <QLineEdit>
#include  <QComboBox>
#include  <QPushButton>
#include  "LButton.h"
#include  <QLabel>
#include  <QCryptographicHash>
#include <QSyntaxHighlighter>
#include <LCore>
#include <QDialog>
#include <QTableView>
#include <QAbstractItemModel>
#include <QTableWidget>
#include <qstandarditemmodel.h>
#include <QStyledItemDelegate>
namespace ljz
{
	struct FindItemInfo
	{
		int lineNumber;
		QString rowText;
		int findBegin;
		int findEnd;
		FindItemInfo() = default;
		FindItemInfo(const int  lineNumber, const QString& rowText, const int findBegin, const int findEnd)
			:lineNumber(lineNumber), rowText(rowText), findBegin(findBegin), findEnd(findEnd)
		{
		}
	};
	class LFindItemDialog :public QDialog
	{
		Q_OBJECT
	public:
		explicit LFindItemDialog(QWidget* parent = nullptr);
		~LFindItemDialog() override = default;
		void setItemInfos(const QStringList& keys, const QList<QVariantMap>& maps);
	signals:
		void findItem(FindItemInfo info);
	private:
		QTableView* _tableView = new QTableView(this);
	};

	class LFindItemDialogDelegate :public QStyledItemDelegate
	{
	public:
		explicit LFindItemDialogDelegate(QWidget* parent = nullptr) :QStyledItemDelegate(parent) {}
		~LFindItemDialogDelegate() override = default;

	private:

		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	};

	/**
	 * 这里是为了处理回车等按键在textEdit 和 lineEdit 中同时触发的问题。
	 */
	class LTextEditKeyPress final :public QObject
	{
		Q_OBJECT
	public:
		explicit LTextEditKeyPress(QObject* parent = nullptr) :QObject(parent) {}
		~LTextEditKeyPress() override {}
	protected:
		bool eventFilter(QObject* watched, QEvent* event) override;
	};

	class NoNewlineValidator : public QValidator {
	public:
		State validate(QString& input, int& pos) const override {
			if (input.contains('\n') || input.contains('\r')) {
				return Invalid;
			}
			return Acceptable;
		}
	};
	class LSearchHighlighter final :public QSyntaxHighlighter
	{
		Q_OBJECT
	public:
		void setSearchBrush(const QBrush& searchBrush)
		{
			_searchBrush = searchBrush;
		}
		void setRe(const QRegularExpression& re)
		{
			_re = re;
		}
		void setSelectCursor(const QTextCursor& selectCursor)
		{
			_selectCursor = selectCursor;
		}
		void setSelectBrush(const QBrush& selectBrush)
		{
			_selectBrush = selectBrush;
		}
		void setNextCursor(const QTextCursor& nextCursor)
		{
			_nextCursor = nextCursor;
		}
		void setNextBrush(const QBrush& nextBrush)
		{
			_nextBrush = nextBrush;
		}
		void setIsHighlight(const bool isHighlight)
		{
			_isHighlight = isHighlight;
		}

		LSearchHighlighter(QWidget* parent = nullptr)
			:QSyntaxHighlighter(parent)
		{
		}
		void highlightBlock(const QString& text) override
		{
			int pos = this->currentBlock().position();
			if (!_isHighlight)
				return;
			QTextCharFormat fmt3;
			fmt3.setBackground(_selectBrush);

			if (!_selectCursor.isNull())
			{
				this->setFormat(std::max(0, _selectCursor.selectionStart() - pos),
					qMin(this->currentBlock().length(), _selectCursor.selectionEnd() - pos)
					, fmt3);
			}
			QRegularExpressionMatchIterator i = _re.globalMatch(text);
			while (i.hasNext())
			{
				QTextCharFormat fmt;
				QRegularExpressionMatch match = i.next();
				int start = qMin(_selectCursor.selectionStart(), _selectCursor.selectionEnd());
				int end = qMax(_selectCursor.selectionStart(), _selectCursor.selectionEnd());

				if (!_nextCursor.isNull() && match.capturedStart() + pos == _nextCursor.selectionStart() && match.capturedEnd() + pos == _nextCursor.selectionEnd())
					fmt.setForeground(_nextBrush);
				else
					fmt.setForeground(_searchBrush);
				if (_selectCursor.isNull()
					|| (match.capturedStart() + pos < start && match.capturedEnd() + pos < start)
					|| (match.capturedStart() + pos > end && match.capturedEnd() + pos > end)
					)
					this->setFormat(match.capturedStart(), match.capturedLength(), fmt);
				else
				{
					if (start > match.capturedStart() + pos)
						this->setFormat(match.capturedStart(), start - match.capturedStart() - pos, fmt);
					if (end < match.capturedEnd() + pos)
						this->setFormat(end, match.capturedEnd() + pos - end, fmt);
					fmt.setBackground(_selectBrush);
					this->setFormat(qMax(match.capturedStart(), start - pos),
						qMin(match.capturedEnd() + pos, end)
						- qMax(match.capturedStart() + pos, start)
						, fmt);
				}
			}
		}
		QBrush _searchBrush;
		QRegularExpression _re;
		QTextCursor _selectCursor;
		QBrush _selectBrush;
		QTextCursor _nextCursor;
		QBrush _nextBrush;
		bool _isHighlight = false;
	};
	class LWIDGET_EXPORT LTextEditToolWidget final :public QFrame
	{
		Q_OBJECT
	public:
		explicit LTextEditToolWidget(QFrame* parent = nullptr);
		~LTextEditToolWidget() override = default;
		void setSearchText(const QString& text);
		void init();
	signals:
		void search(const QString& text, QTextDocument::FindFlags findFlags, bool isRegExp = false);
		void searchMove(const QString& text, QTextDocument::FindFlags findFlags, bool isRegExp = false);
		void replace(const QString& text, const QString& replaceText, QTextDocument::FindFlags findFlags, bool isRegExp = false);
		void replaceAll(const QString& text, const QString& replaceText, QTextDocument::FindFlags findFlags, bool isRegExp = false);
		void seeFindResult(const QString& text, QTextDocument::FindFlags findFlags, bool isRegExp = false);
	private:
		QLineEdit* _searchLineEdit = new QLineEdit(this);
		QPushButton* _searchButton = new QPushButton(this);
		QComboBox* _comboBox = new QComboBox(this);
		LSwitchButton* _caseSensitiveButton = new LSwitchButton(this);
		LSwitchButton* _wholeWordButton = new LSwitchButton(this);
		LSwitchButton* _regExpButton = new LSwitchButton(this);
		void focusOutEvent(QFocusEvent* event) override;
		QTextDocument::FindFlags createFlags() const;
		QPushButton* _button = new QPushButton(this);
		QPushButton* _seeButton = new QPushButton(this);
		QPushButton* _replaceNextButton = new QPushButton("替换下一个");
		QPushButton* _replaceAllButton = new QPushButton("全部替换");
		QLineEdit* _replaceLineEdit = new QLineEdit();
		void focusInEvent(QFocusEvent* event) override;
		void keyReleaseEvent(QKeyEvent* event) override;
	};

	class LWIDGET_EXPORT LTextEdit final :public QTextEdit
	{
		Q_OBJECT
	public:
		struct ColorInfo
		{
			QBrush allFindBrush = QColor(191, 0, 255);
			QBrush nextFindBrush = QColor(Qt::red);
			QBrush rangeFindBrush = QColor(135, 206, 235);
			ColorInfo() = default;
		};
		explicit LTextEdit(QWidget* parent = nullptr);
		void copySelectCursor(QTextCursor cursor);
		~LTextEdit() override = default;
		void search(const QString& text, QTextDocument::FindFlags findFlags, bool isRegExp = false);
		void searchMove(const QString& text, QTextDocument::FindFlags findFlags, bool isRegExp = false);
		void replace(const QString& text, const QString& replaceText, QTextDocument::FindFlags findFlags, bool isRegExp = false);
		void replaceAll(const QString& text, const QString& replaceText, QTextDocument::FindFlags findFlags, bool isRegExp = false);
		LTextEditToolWidget* searchToolWidget() const { return _searchToolWidget; }
		void setColorInfo(const ColorInfo& colorInfo) { _colorInfo = colorInfo; }
		void showFindResult(const QString& text, QTextDocument::FindFlags findFlags, bool isRegExp = false);
	signals:
		void notFind();
	private:
		LTextEditToolWidget* _searchToolWidget{ new LTextEditToolWidget(this) };
		void keyPressEvent(QKeyEvent* event) override;
		void resizeEvent(QResizeEvent* event) override;
		QTextCursor _selectCursor;
		QVariant _findData = QVariant();
		QTextDocument::FindFlags _findFlags;
		bool _isRegExp = false;
		QTextCursor _lastCursor;
		void focusInEvent(QFocusEvent* event) override;
		void focusOutEvent(QFocusEvent* event) override;
		ColorInfo _colorInfo;
		QString _hashText;
		void textChangedSlot();
		LSearchHighlighter* _highlighter = new LSearchHighlighter(this);
		void paintEvent(QPaintEvent* event) override;
		void findItemClicked(FindItemInfo info);
		LFindItemDialog* dialog = nullptr;
	};
}
