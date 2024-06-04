from typing import Any, overload

from PySide6.QtCore import QObject, QEvent, Signal, Slot, Qt, QRect
from PySide6.QtGui import QStandardItemModel, QMouseEvent, QKeyEvent, QStandardItem, QCursor
from PySide6.QtWidgets import QComboBox, QListView, QLineEdit, QWidget


class KeyPressEater(QObject):
    activated = Signal(int)

    def __init__(self, parent: QObject = None):
        super().__init__(parent)

    def eventFilter(self, watched: QObject, event: QEvent) -> bool:
        if event.type() == QEvent.Type.KeyPress:
            keyEvent: QKeyEvent = event
            if keyEvent.key() == Qt.Key.Key_Space:
                if isinstance(watched, QListView):
                    listView: QListView = watched
                    index = listView.currentIndex().row()
                    if index != -1:
                        self.activated.emit(index)
            elif keyEvent.key() in [Qt.Key.Key_Up, Qt.Key.Key_Down]:
                return super().eventFilter(watched, event)
            return True
        else:
            return super().eventFilter(watched, event)


class LMultiComboBox(QComboBox):
    showedPopup = Signal()
    hidedPopup = Signal()
    itemStateChanged = Signal()

    class ItemInfo:
        def __init__(self, index: int = -1, text: str = "", data: Any = None, isCheck: bool = False):
            self.isCheck = isCheck
            self.data = data
            self.text = text
            self.index = index

    def __init__(self, parent: QWidget = None):
        super().__init__(parent)
        self._lineEdit = QLineEdit()
        self._listView = QListView()
        self._model = QStandardItemModel()
        self._lineEdit.setReadOnly(True)
        self.setLineEdit(self._lineEdit)
        keyPressEater = KeyPressEater(self)
        self._listView.installEventFilter(keyPressEater)
        self.setView(self._listView)
        self._listView.setModel(self._model)
        self.activated.connect(self.selectActivated)
        keyPressEater.activated.connect(self.selectActivated)

    def updateText(self):
        strList = []
        for i in range(self._model.rowCount()):
            item = self._model.item(i)
            if item.checkState() == Qt.CheckState.Checked:
                strList.append(item.text())
        self._lineEdit.setText(",".join(strList))
        self._lineEdit.setToolTip("\n".join(strList))

    def addItem(self, text: str, isCheck: bool = False, data: Any = None):
        item = QStandardItem(text)
        item.setCheckable(True)
        item.setCheckState(Qt.CheckState.Checked if isCheck
                           else Qt.CheckState.Unchecked)
        item.setData(str, role=Qt.ItemDataRole.UserRole + 1)
        self._model.appendRow(item)
        self.updateText()

    @overload
    def addItems(self, items: list[ItemInfo]):
        pass

    @overload
    def addItems(self, items: dict[str, bool]):
        pass

    @overload
    def addItems(self, items: list[str]):
        pass

    def addItems(self, *args, **kwargs):
        if len(args) == 1:
            if isinstance(args[0], list):
                for item in args[0]:
                    if isinstance(item, self.ItemInfo):
                        self.addItem(item.text, item.isCheck, item.data)
                    else:
                        self.addItem(item)
            elif isinstance(args[0], dict):
                for key, value in args[0].items():
                    self.addItem(key, value)
            else:
                raise ValueError("Unsupported type")

    def removeItem(self, index):
        self._model.removeRow(index)
        self.updateText()

    def clear(self):
        self._model.clear()
        self.updateText()

    def selectedItemsText(self) -> list[str]:
        return self._lineEdit.text().split(",")

    def selectedItems(self) -> list[ItemInfo]:
        items = []
        for i in range(self._model.rowCount()):
            item = self._model.item(i)
            if item.checkState() == Qt.CheckState.Checked:
                itemInfo = self.ItemInfo(i, item.text(), item.data(Qt.ItemDataRole.UserRole + 1), True)
                items.append(itemInfo)
        return items

    def itemText(self, index: int) -> str:
        if 0 <= index < self._model.rowCount():
            return self._model.item(index).text()
        else:
            return ""

    def itemInfo(self, index: int) -> ItemInfo:
        itemInfo = self.ItemInfo()
        if 0 <= index < self._model.rowCount():
            item = self._model.item(index)
            itemInfo.index = index
            itemInfo.text = item.text()
            itemInfo.data = item.data(Qt.ItemDataRole.UserRole + 1)
            itemInfo.isCheck = item.checkState() == Qt.CheckState.Checked
        return itemInfo

    def findItem(self, data: Any) -> int:
        for i in range(self._model.rowCount()):
            item = self._model.item(i)
            if item.data(Qt.ItemDataRole.UserRole + 1) == data:
                return i
        return -1

    def showPopup(self):
        self.showedPopup.emit()
        super().showPopup()

    def hidePopup(self):
        width = self.view().width()
        height = self.view().height()
        x = QCursor.pos().x() - self.mapToGlobal(self.geometry().topLeft()).x() + self.geometry().x()
        y = QCursor.pos().y() - self.mapToGlobal(self.geometry().topLeft()).y() + self.geometry().y()
        rect = QRect(0, self.height(), width, height)
        if not rect.contains(x, y):
            self.hidedPopup.emit()
            super().hidePopup()

    def mousePressEvent(self, e: QMouseEvent):
        super().mousePressEvent(e)
        e.accept()

    def mouseReleaseEvent(self, e: QMouseEvent):
        super().mouseReleaseEvent(e)
        e.accept()

    def mouseMoveEvent(self, event: QMouseEvent):
        super().mouseMoveEvent(event)
        event.accept()

    @Slot(int)
    def selectActivated(self, index: int):
        item = self._model.item(index)
        item.setCheckState(Qt.CheckState.Checked if item.checkState() == Qt.CheckState.Unchecked
                           else Qt.CheckState.Unchecked)
        self.updateText()
        self.itemStateChanged.emit()

