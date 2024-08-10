#pragma once
#include "lwidget_global.h"
#include <QComboBox>
#include <QStandardItemModel>
#include <QListView>
#include <QMouseEvent>
#include <QEvent>
#include <QCompleter>
#include "LLineedit.h"

namespace ljz
{
	class KeyPressEater :public QObject
	{
		Q_OBJECT
	public:
		explicit KeyPressEater(QObject* parent = nullptr) :QObject(parent) {}
		~KeyPressEater() override {}
	signals:
		void activated(int index);
	protected:
		bool eventFilter(QObject* watched, QEvent* event) override;
	};

	/**
	 * 多选下拉列表
	 */
	class LWIDGET_EXPORT LMultiComboBox :public QComboBox
	{
		Q_OBJECT
	public:
		struct ItemInfo
		{
			int index = -1;
			QString text = QString();
			QVariant data = QVariant();
			bool isCheck = false;
			ItemInfo() = default;
			ItemInfo(const int index, QString text, QVariant data, const bool isCheck = false) : index(index), text(std::move(text)), data(std::move(data)), isCheck(isCheck) {}
		};
		explicit LMultiComboBox(QWidget* parent = nullptr);
		~LMultiComboBox() override = default;
		void addItem(const QString& text, const bool isCheck = false, const QVariant& data = QVariant());
		void addItems(const QList<ItemInfo>& items);
		void addItems(const QMap<QString, bool>& items);
		void addItems(const QStringList& items);
		void removeItem(const int index);
		void clear();
		QStringList selectedItemsText() const;
		QList<ItemInfo> selectedItems() const;
		QString itemText(const int index) const;
		ItemInfo itemInfo(const int index) const;
		int findItem(const QVariant& data) const;

	signals:
		void showedPopup();
		void hidedPopup();
		void itemStateChanged();
	protected:
		void showPopup() override;
		void hidePopup() override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
	private:
		void updateText() const;
	private slots:
		void selectActivated(int index);
	private:
		QLineEdit* _lineEdit{ new QLineEdit(this) };
		QListView* _listView{ new QListView(this) };
		QStandardItemModel _model;
	};

	/**
	 * 补全下拉列表
	 */
	class LWIDGET_EXPORT LCompleteComboBox :public QComboBox
	{
		Q_OBJECT
	public:
		explicit LCompleteComboBox(QWidget* parent = nullptr);

		~LCompleteComboBox() override = default;

		void setFilterMode(Qt::MatchFlags mode) const;

		Qt::MatchFlags filterMode() const;

		void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity) const;

		Qt::CaseSensitivity caseSensitivity() const;
	private:
		QCompleter* _completer;
		void paintEvent(QPaintEvent* e) override;
	};
	typedef  QList<QHostAddress> QHostAddressList;
	class LHostAddressCompleter :public QCompleter
	{
		Q_OBJECT
	public:
		explicit LHostAddressCompleter(QWidget* parent = nullptr);
	protected:
		QStringList splitPath(const QString& path) const override;
	};

	class LWIDGET_EXPORT LHostAddressComboBox :public QComboBox
	{
		Q_OBJECT
	public:
		explicit LHostAddressComboBox(bool loadAllInterfaces, QWidget* parent = nullptr);
		explicit LHostAddressComboBox(QWidget* parent = nullptr);
		~LHostAddressComboBox() override;
		void addItem(const QHostAddress& address);
		void addItem(const QString& address);
		void addItems(const QHostAddressList& addresses);
		void addItems(const QStringList& addresses);
		void removeItem(const int index);
		void insertItem(const int index, const QHostAddress& address);
		void insertItem(const int index, const QString& address);
		void insertItems(const int index, const QHostAddressList& addresses);
		void insertItems(const int index, const QStringList& addresses);
		[[nodiscard]] QHostAddress currentHostAddress() const;
		void setCurrentHostAddress(const QHostAddress& address);
		int findHostAddress(const QString& address);
		int findHostAddress(const QHostAddress& address);
		[[nodiscard]] QHostAddress itemHostAddress(int index) const;
		[[nodiscard]] LHostAddressEdit* lineEdit() const;
		void setLineEdit(LHostAddressEdit* edit);
		[[nodiscard]] QStringList hostAddressStringList() const;
		[[nodiscard]] QHostAddressList hostAddressList() const;
	public slots:

	signals:
		void currentHostAddressChanged(const QHostAddress& address);
	private:
		LHostAddressEdit* _edit = new LHostAddressEdit(this);
		LHostAddressCompleter* _complter = new LHostAddressCompleter(this);
	};
}
