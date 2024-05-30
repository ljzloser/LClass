#pragma once
#include "lwidget_global.h"
#include "LButton.h"
#include <QLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QCryptographicHash>
#include <QSyntaxHighlighter>
#include <QDialog>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPlainTextEdit>

namespace ljz
{
	class LLineNumberArea;
	struct FindItemInfo
	{
		int lineNumber;
		QString rowText;
		int findBegin;
		int findEnd;
		FindItemInfo() = default;
		FindItemInfo(const int  lineNumber, const QString& rowText, const int findBegin, const int findEnd)
			:lineNumber(lineNumber), rowText(rowText), findBegin(findBegin), findEnd(findEnd) {}
	};
	class LFindItemDialog :public QDialog
	{
		Q_OBJECT
	public:
		explicit LFindItemDialog(QWidget* parent = nullptr);
		~LFindItemDialog() override = default;
		void setItemInfos(const QStringList& keys, const QList<QVariantMap>& maps) const;
	signals:
		void findItem(FindItemInfo info);
	private:
		QTableView* _tableView = new QTableView(this);
		QLabel* _label = new QLabel(this);
	};

	class LFindItemDialogDelegate final :public QStyledItemDelegate
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
		~LTextEditKeyPress() override = default;

	protected:
		bool eventFilter(QObject* watched, QEvent* event) override;
	};

	class NoNewlineValidator : public QValidator
	{
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
		void setFindGround(const QBrush& findGround)
		{
			_findGround = findGround;
		}
		void setIsHighlight(const bool isHighlight)
		{
			_isHighlight = isHighlight;
		}
		explicit LSearchHighlighter(QTextDocument* parent = nullptr)
			:QSyntaxHighlighter(parent)
		{
		}
		void highlightBlock(const QString& text) override;
		QBrush _searchBrush;
		QRegularExpression _re;
		QTextCursor _selectCursor;
		QBrush _selectBrush;
		QTextCursor _nextCursor;
		QBrush _nextBrush;
		QBrush _findGround;
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
		QPushButton* _button = new QPushButton(this);
		QPushButton* _seeButton = new QPushButton(this);
		QPushButton* _replaceNextButton = new QPushButton("替换下一个");
		QPushButton* _replaceAllButton = new QPushButton("全部替换");
		QLineEdit* _replaceLineEdit = new QLineEdit();
		QTextDocument::FindFlags createFlags() const;
		void focusOutEvent(QFocusEvent* event) override;
		void focusInEvent(QFocusEvent* event) override;
		void keyReleaseEvent(QKeyEvent* event) override;
		void showEvent(QShowEvent* event) override;
	};

	class LWIDGET_EXPORT LTextEdit final :public QPlainTextEdit
	{
		Q_OBJECT
	public:
		struct ColorInfo
		{
			QBrush allFindBrush = QColor(Qt::darkGreen);
			QBrush nextFindBrush = QColor(Qt::red);
			QBrush rangeFindBrush = QColor(135, 206, 235);
			QBrush findGroundBrush = QColor(Qt::yellow);
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
	public:
		void lineNumberAreaPaintEvent(QPaintEvent* event);
		int lineNumberAreaWidth() const;
		void updateLineNumberAreaWidth(int newBlockCount);
		void highlightCurrentLine();
		void updateLineNumberArea(const QRect& rect, int dy);
		void selectLineByPoint(QPoint point);
		void setLineNumberAreaVisible(bool visible) const;
		bool isLineNumberAreaVisible() const;
		void setHighlightCurrentLine(bool highlight);
		bool isHighlightCurrentLine() const;
	private:
		LTextEditToolWidget* _searchToolWidget{ new LTextEditToolWidget(this) };
		QTextCursor _selectCursor;
		QVariant _findData = QVariant();
		QTextDocument::FindFlags _findFlags;
		bool _isRegExp = false;
		QTextCursor _lastCursor;
		ColorInfo _colorInfo;
		QString _hashText;
		LSearchHighlighter* _highlighter = new LSearchHighlighter(this->document());
		LFindItemDialog* _dialog = nullptr;
		LLineNumberArea* _lineNumberArea = nullptr;
		bool _isHighlightCurrentLine = false;
		void keyPressEvent(QKeyEvent* event) override;
		void resizeEvent(QResizeEvent* event) override;
		void focusInEvent(QFocusEvent* event) override;
		void focusOutEvent(QFocusEvent* event) override;
		void textChangedSlot();
		void paintEvent(QPaintEvent* event) override;
		void findItemClicked(FindItemInfo info);
	};
	class LWIDGET_EXPORT LLineNumberArea final : public QWidget
	{
	public:
		LLineNumberArea(LTextEdit* editor) : QWidget(editor), codeEditor(editor) {}

		QSize sizeHint() const override { return { codeEditor->lineNumberAreaWidth(), 0 }; }
		// 重写鼠标点击事件
		void mousePressEvent(QMouseEvent* event) override
		{
			//如果是左键
			if (event->button() == Qt::LeftButton)
			{
				//获取鼠标点击的坐标
				QPoint point = event->pos();
				//根据坐标选中一行
				codeEditor->selectLineByPoint(point);
			}
		}
	protected:
		void paintEvent(QPaintEvent* event) override
		{
			codeEditor->lineNumberAreaPaintEvent(event);
		}

	public:
		LTextEdit* codeEditor;
	};
}
