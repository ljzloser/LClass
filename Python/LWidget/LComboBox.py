from typing import Any, overload

from PySide6.QtCore import QObject, QEvent, Signal, Slot, Qt, QRect
from PySide6.QtGui import QStandardItemModel, QMouseEvent, QKeyEvent, QStandardItem, QCursor
from PySide6.QtWidgets import QComboBox, QListView, QLineEdit, QWidget, QCompleter


class KeyPressEater(QObject):
    activated = Signal(int)

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
        def __init__(self, index=-1, text="", data=None, isCheck=False):
            self.index = index
            self.text = text
            self.data = data
            self.isCheck = isCheck

    def __init__(self, parent: QWidget = None):
        super().__init__(parent)
        self._model = QStandardItemModel(self)
        self._lineEdit = QLineEdit(self)
        self._listView = QListView(self)
        self.setModel(self._model)
        self.setLineEdit(self._lineEdit)
        self.setView(self._listView)
        self._keyPressEater = KeyPressEater(self)
        self._listView.installEventFilter(self._keyPressEater)
        self.activated.connect(self.selectActivated)
        self._keyPressEater.activated.connect(self.selectActivated)

    def addItem(self, text: str, isCheck: bool = False, data: Any = None):
        item = QStandardItem(text)
        item.setCheckable(True)
        item.setCheckState(Qt.CheckState.Checked if isCheck else Qt.CheckState.Unchecked)
        item.setData(data)
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

    def addItems(self, items):
        if isinstance(items, (list, tuple)):
            for item in items:
                if isinstance(item, self.ItemInfo):
                    self.addItem(item.text, item.isCheck, item.data)
                elif isinstance(item, str):
                    self.addItem(item)
        elif isinstance(items, dict):
            for text, isCheck in items.items():
                self.addItem(text, isCheck)

    def removeItem(self, index: int):
        self._model.removeRow(index)
        self.updateText()

    def clear(self):
        self._model.clear()
        self.updateText()

    def selectedItemsText(self) -> list[str]:
        return [self._model.item(i).text() for i in range(self._model.rowCount()) if
                self._model.item(i).checkState() == Qt.CheckState.Checked]

    def selectedItems(self) -> list[ItemInfo]:
        return [self.ItemInfo(i, self._model.item(i).text(), self._model.item(i).data(),
                              self._model.item(i).checkState() == Qt.CheckState.Checked) for i in
                range(self._model.rowCount()) if self._model.item(i).checkState() == Qt.CheckState.Checked]

    def itemText(self, index) -> str:
        return self._model.item(index).text()

    def itemInfo(self, index) -> ItemInfo:
        item = self._model.item(index)
        return self.ItemInfo(index, item.text(), item.data(), item.checkState() == Qt.CheckState.Checked)

    def findItem(self, data: Any) -> int:
        for i in range(self._model.rowCount()):
            if self._model.item(i).data() == data:
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

    def mousePressEvent(self, event: QMouseEvent):
        super().mousePressEvent(event)
        event.accept()

    def mouseReleaseEvent(self, event: QMouseEvent):
        super().mouseReleaseEvent(event)
        event.accept()

    def mouseMoveEvent(self, event: QMouseEvent):
        super().mouseMoveEvent(event)
        event.accept()

    def updateText(self):
        selected = [self._model.item(i).text() for i in range(self._model.rowCount()) if
                    self._model.item(i).checkState() == Qt.CheckState.Checked]
        self._lineEdit.setText(",".join(selected))
        self._lineEdit.setToolTip("\n".join(selected))

    @Slot(int)
    def selectActivated(self, index):
        item = self._model.item(index)
        item.setCheckState(
            Qt.CheckState.Unchecked if item.checkState() == Qt.CheckState.Checked else Qt.CheckState.Checked)
        self.updateText()
        self.itemStateChanged.emit()


class LCompleteComboBox(QComboBox):
    def __init__(self, parent=None):
        super().__init__(parent)
        self._completer = QCompleter()
        self.setEditable(True)
        self.setCompleter(self._completer)
        self._completer.setFilterMode(Qt.MatchContains)
        self._completer.setCaseSensitivity(Qt.CaseInsensitive)
        self._completer.setModel(self.model())

    def setFilterMode(self, mode: Qt.MatchFlag):
        self._completer.setFilterMode(mode)

    def filterMode(self) -> Qt.MatchFlag:
        return self._completer.filterMode()

    def setCaseSensitivity(self, caseSensitivity: Qt.CaseSensitivity):
        self._completer.setCaseSensitivity(caseSensitivity)

    def caseSensitivity(self) -> Qt.CaseSensitivity:
        return self._completer.caseSensitivity()

    def paintEvent(self, event):
        super().paintEvent(event)
