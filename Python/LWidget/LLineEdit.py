from typing import overload, List
from PySide6.QtWidgets import *
from PySide6.QtCore import *
from PySide6.QtGui import *
from PySide6.QtNetwork import *


class LFileLineEdit(QLineEdit):
    fileSelected = Signal(list, str)
    fileCleared = Signal()

    class Info:
        def __init__(self, mode: QFileDialog.FileMode = QFileDialog.FileMode.ExistingFile,
                     path: str = QApplication.applicationFilePath(),
                     title: str = "请选择文件",
                     filters=None,
                     icon=None,
                     ):
            if filters is None:
                filters = ["All Files (*)"]
            if icon is None:
                self.icon = QIcon(":/res/res/file.png")
            self.mode = mode
            self.path = path
            self.filters = filters
            self.title = title

    @overload
    def __init__(self, parent: QWidget = None):
        ...

    @overload
    def __init__(self, path: str, parent: QWidget = None):
        ...

    def __init__(self, *args, **kwargs):
        if len(args) == 0:
            super().__init__()
        elif len(args) == 1:
            parent = args[0]
            super().__init__(parent)
        else:
            parent = args[1]
            text = args[0]
            super().__init__(text, parent)

        self._action: QAction = QAction()
        self._clearAction: QAction = QAction()
        self._info = LFileLineEdit.Info()
        self.setReadOnly(True)
        self._clearAction.setIcon(QIcon(":/res/res/clear.png"))
        self._clearAction.setToolTip("清空")
        self.addAction(self._clearAction, QLineEdit.ActionPosition.TrailingPosition)
        self._clearAction.triggered.connect(self.clearActionTrigger)
        self.reAction()

    def action(self) -> QAction:
        return self._action

    def clearAction(self) -> QAction:
        return self._clearAction

    def info(self) -> Info:
        return self._info

    def setInfo(self, info: Info):
        self._info = info
        self.reAction()

    @overload
    def selectedFiles(self) -> List[str]:
        ...

    @overload
    def selectedFiles(self, sep: str) -> str:
        ...

    def selectedFiles(self, *args, **kwargs):
        if len(args) == 0 and len(kwargs) == 0:
            return self.text().split(";")
        else:
            return self.text().join(args[0])

    def setClearActionVisible(self, visible: bool):
        self._clearAction.setVisible(visible)

    def clearActionVisible(self) -> bool:
        return self._clearAction.isVisible()

    @Slot()
    def showFileDialog(self):
        fileDialog = QFileDialog(None, self._info.title, self._info.path)
        fileDialog.setNameFilters(self._info.filters)
        fileDialog.setFileMode(self._info.mode)
        if fileDialog.exec() == QFileDialog.DialogCode.Accepted:
            fileList = fileDialog.selectedFiles()
            filters = fileDialog.selectedNameFilter()
            if fileList:
                self.setText(";".join(fileList))
                self.setToolTip("\n".join(fileList))
                self.fileSelected.emit(fileList, filters)

    def reAction(self):
        self._action.deleteLater()
        self._action = None
        self._action = QAction()
        self._action.setIcon(self._info.icon)
        self.setPlaceholderText(self._info.title)
        self._action.setToolTip(self._info.title)
        self.addAction(self._action, QLineEdit.ActionPosition.TrailingPosition)
        self._action.triggered.connect(self.showFileDialog)

    def clearActionTrigger(self):
        self.clear()
        self.setToolTip("")
        self.fileCleared.emit()


class LFocusSelectLineEdit(QLineEdit):
    def __init__(self, text="", parent=None):
        super().__init__(text, parent)

    def focusInEvent(self, arg__1: QFocusEvent):
        super().focusInEvent(arg__1)
        QTimer.singleShot(100, lambda: self.selectAll())


class LHostAddressValidator(QValidator):
    def __init__(self, parent=None):
        super().__init__(parent)

    def validate(self, inputText: str, pos: int) -> QValidator.State:
        regularExpression = QRegularExpression(
            "^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])?\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])?$")
        match: QRegularExpressionMatch = regularExpression.match(inputText)
        if match.hasMatch():
            return QValidator.State.Acceptable
        return QValidator.State.Invalid


class LHostAddressEdit(QLineEdit):
    currentBlockChanged = Signal(int)
    hostAddressChanged = Signal(QHostAddress)
    hostAddressEditFinished = Signal(QHostAddress)

    @overload
    def __init__(self, text: str = "", parent: QWidget | None = None) -> None:
        ...

    @overload
    def __init__(self, parent: QWidget | None = None) -> None:
        ...

    @overload
    def __init__(self, hostAddress: QHostAddress, parent: QWidget | None = None):
        ...

    def __init__(self, *args, **kwargs):
        self._hostAddress = QHostAddress()
        self._currentblock = 0
        self._defaultblock = 0
        parent = None
        hostAddress = QHostAddress()
        if len(args) == 1:
            parent = args[0]
        elif len(args) == 2:
            if isinstance(args[0], QHostAddress):
                hostAddress = args[0]
            else:
                hostAddress = QHostAddress(args[0])
            parent = args[1]
        super().__init__(parent)
        self.setValidator(LHostAddressValidator(self))
        self.cursorPositionChanged.connect(self.handleCursorPositionChanged)
        self.selectionChanged.connect(self.handleSelectionChanged)
        self.setHostAddress(hostAddress)

    def hostAddress(self) -> QHostAddress:
        return self._hostAddress

    def currentBlock(self) -> int:
        return self._currentblock

    def defaultBlock(self) -> int:
        return self._defaultblock

    @Slot(str)
    def setText(self, text: str):
        self.setHostAddress(QHostAddress(text))

    @Slot()
    def hostAddressEditFinish(self):
        self.setHostAddress(QHostAddress(self.text()))
        self.hostAddressEditFinished.emit(self._hostAddress)

    @Slot()
    def nextBlock(self):
        self.setCurrentBlock(self._currentblock + 1)

    @Slot(int)
    def setCurrentBlock(self, block: int):
        if 0 <= block <= 3:
            self.setHostAddress(QHostAddress(self.text()))
            ipv4: list = self.text().split(".")
            if len(ipv4[self._currentblock]) == 0:
                ipv4[self._currentblock] = len(self._hostAddress.toString().split(".")[self._currentblock])
                super().setText(".".join(ipv4))
            self._currentblock = block
            beginPos = self._currentblock + sum([len(ipv4[i]) for i in range(self._currentblock)])
            if self.hasFocus():
                self.setSelection(beginPos, len(ipv4[self._currentblock]))
            self.currentBlockChanged.emit(self._currentblock)

    @Slot(int)
    def setDefaultBlock(self, block: int):
        if 0 <= block <= 3:
            self._defaultblock = block

    @Slot(QHostAddress)
    def setHostAddress(self, hostAddress: QHostAddress):
        if hostAddress.isNull() or hostAddress.toString() == self._hostAddress.toString():
            if self.text() != self._hostAddress.toString():
                super().setText(self._hostAddress.toString())
                return
        self._hostAddress = hostAddress
        super().setText(self._hostAddress.toString())
        self.hostAddressChanged.emit(self._hostAddress)

    def focusOutEvent(self, event: QFocusEvent) -> None:
        super().focusOutEvent(event)
        self.hostAddressEditFinish()

    def focusInEvent(self, event: QFocusEvent) -> None:
        super().focusInEvent(event)
        QTimer.singleShot(100, lambda: self.setCurrentBlock(self._defaultblock))

    def keyPressEvent(self, event: QKeyEvent) -> None:
        if self.hasSelectedText():
            if event.key() == Qt.Key.Key_Left:
                self.setCurrentBlock(self._currentblock - 1)
                return
            elif event.key() == Qt.Key.Key_Right:
                self.setCurrentBlock(self._currentblock + 1)
                return
        super().keyPressEvent(event)
        if event.key() == Qt.Key.Key_Period:
            self.nextBlock()

    def handleCursorPositionChanged(self, oldPosition: int, newPosition: int) -> None:
        if self.hasSelectedText() and self.selectedText() == self.text():
            return
        ipv4 = self.text().split(".")
        pos = newPosition
        newBlock = 0

        if pos <= len(ipv4[0]):
            newBlock = 0
        elif pos <= len(ipv4[0]) + len(ipv4[1]) + 1:
            newBlock = 1
        elif pos <= len(ipv4[0]) + len(ipv4[1]) + len(ipv4[2]) + 2:
            newBlock = 2
        elif pos <= len(ipv4[0]) + len(ipv4[1]) + len(ipv4[2]) + len(ipv4[3]) + 3:
            newBlock = 3
        else:
            newBlock = 0
        if newBlock != self._currentblock:
            self.setCurrentBlock(newBlock)

    def handleSelectionChanged(self) -> None:
        if self.selectedText() == ".":
            self.setSelection(self.cursorPosition(), 0)
