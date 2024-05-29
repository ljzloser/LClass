#ifndef LLINEEDIT_H
#define LLINEEDIT_H

#include <QLineEdit>
#include "lwidget_global.h"
#include <QApplication>
#include <QFileDialog>
namespace ljz {
	class LWIDGET_EXPORT LFileLineEdit final : public QLineEdit
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
}

#endif // LLINEEDIT_H
