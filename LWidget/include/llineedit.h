#ifndef LLINEEDIT_H
#define LLINEEDIT_H

#include "lwidget_global.h"
#include <QLineEdit>
#include <QApplication>
#include <QFileDialog>
#include <QHostAddress>
#include <QAbstractSpinBox>
#include <QValidator>
namespace ljz {
	class LWIDGET_EXPORT LFileLineEdit : public QLineEdit
	{
		Q_OBJECT
	public:
		struct Info
		{
			QFileDialog::FileMode mode = QFileDialog::ExistingFile;
			QString path = QApplication::applicationDirPath();
			QString title = "请选择文件";
			QStringList filters = { "All Files (*)" };
			QIcon icon = QIcon(":/res/res/file.png");
			Info() = default;
			Info(QFileDialog::FileMode mode, const QString& path, const QString& title, const QStringList& filters, const QIcon& icon);
		};
	public:
		explicit LFileLineEdit(QWidget* parent = nullptr);
		explicit LFileLineEdit(const QString& text, QWidget* parent = nullptr);
		~LFileLineEdit() override = default;
		QAction* action() const;
		QAction* clearAction() const;
		[[nodiscard]] Info info() const;

		void setInfo(const Info& info);
		QStringList selectedFiles() const;
		QString selectedFiles(const QString& sep) const;
		void setClearActionVisible(bool visible) const;
		bool clearActionVisible() const;
	signals:
		void fileSelected(const QStringList& files, const QString& filter);
		void fileCleared();
	public slots:
		void showFileDialog();

	private:
		QAction* _action{ new QAction() };
		QAction* _clearAction{ new QAction() };
		Info _info;
		void reAction();
		void init();
	};
	class LWIDGET_EXPORT LFocusSelectLineEdit : public QLineEdit
	{
		Q_OBJECT
	public:
		explicit LFocusSelectLineEdit(QWidget* parent = nullptr);
		explicit LFocusSelectLineEdit(const QString& text, QWidget* parent = nullptr);
		~LFocusSelectLineEdit() override = default;
	protected:
		void focusInEvent(QFocusEvent* event) override;
	};
	class LHostAddressValidator : public QValidator
	{
		Q_OBJECT
	public:
		explicit LHostAddressValidator(QObject* parent = nullptr);
		~LHostAddressValidator() override;
		State validate(QString& input, int& pos) const override;
	};
	class LWIDGET_EXPORT LHostAddressLineEdit : public QLineEdit
	{
		Q_OBJECT
	public:
		explicit LHostAddressLineEdit(const QHostAddress& hostAddress, QWidget* parent = nullptr);
		explicit LHostAddressLineEdit(const QString& text, QWidget* parent = nullptr);
		explicit LHostAddressLineEdit(QWidget* parent = nullptr);
		[[nodiscard]] QHostAddress hostAddress() const;
		[[nodiscard]] int currentBlock() const;
		[[nodiscard]] int defaultBlock() const;

	signals:
		void currentBlockChanged(int block);
		void currentHostAddressChanged(const QHostAddress& hostAddress);
	public slots:
		void setText(const QString& text);
		void newHostAddress();
		void nextBlock();
		void setCurrentBlock(const int block);
		void setDefaultBlock(const int block = 0);
		void setHostAddress(const QHostAddress& hostAddress);
	protected:
		void focusOutEvent(QFocusEvent* event) override;
		void focusInEvent(QFocusEvent* event) override;
		void keyPressEvent(QKeyEvent* event) override;
		void handleCursorPositionChanged(int oldPos, int newPos);
	private:
		QHostAddress _hostAddress;
		int _currentblock{ 0 };
		int _defaultblock{ 0 };
	};
}

#endif // LLINEEDIT_H
