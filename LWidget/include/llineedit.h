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
			QFileDialog::FileMode mode = QFileDialog::ExistingFile; // 文件选择对话框的文件模式
			QString path = QApplication::applicationDirPath(); // 文件选择对话框的初始路径
			QString title = "请选择文件"; // 文件选择对话框的标题
			QStringList filters = { "All Files (*)" }; // 文件选择对话框的文件过滤器
			QIcon icon = QIcon(":/res/res/file.png"); // 文件选择对话框的图标
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
	class LWIDGET_EXPORT LHostAddressEdit : public QLineEdit
	{
		Q_OBJECT
	public:
		explicit LHostAddressEdit(const QHostAddress& hostAddress, QWidget* parent = nullptr);
		explicit LHostAddressEdit(const QString& text, QWidget* parent = nullptr);
		explicit LHostAddressEdit(QWidget* parent = nullptr);
		[[nodiscard]] QHostAddress hostAddress() const;
		[[nodiscard]] int currentBlock() const;
		[[nodiscard]] int defaultBlock() const;

	signals:
		void currentBlockChanged(int block);
		void hostAddressChanged(const QHostAddress& hostAddress);
		void hostAddressEditFinished(const QHostAddress& hostAddress);
	public slots:
		void setText(const QString& text);
		void hostAddressEditFinish();
		void nextBlock();
		void setCurrentBlock(int block);
		void setDefaultBlock(int block = 0);
		void setHostAddress(const QHostAddress& hostAddress);
	protected:
		void focusOutEvent(QFocusEvent* event) override;
		void focusInEvent(QFocusEvent* event) override;
		void keyPressEvent(QKeyEvent* event) override;
		void handleCursorPositionChanged(int oldPos, int newPos);
	private:
		QHostAddress _hostAddress{ "0.0.0.0" };
		int _currentblock{ 0 };
		int _defaultblock{ 0 };
	};
}

#endif // LLINEEDIT_H
