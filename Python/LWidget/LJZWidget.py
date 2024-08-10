from PySide6.QtWidgets import *
from PySide6.QtCore import *
from PySide6.QtGui import *
from win32api import *

from PySide6.QtCore import Qt, Signal, Slot
from PySide6.QtGui import QIcon, QPixmap, QPainter, QMouseEvent, QColor, QFocusEvent
from PySide6.QtWidgets import QWidget, QPushButton, QHBoxLayout, QVBoxLayout, QSpacerItem, QSizePolicy
from enum import Enum


class LBaseTitleBar(QWidget):
    closeButtonClicked = Signal()
    minButtonClicked = Signal()

    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

    def getMaxButton(self):
        raise NotImplementedError

    def updateIcon(self):
        raise NotImplementedError

    def closeButtonClick(self):
        raise NotImplementedError

    def minButtonClick(self):
        raise NotImplementedError


class LTitleBar(LBaseTitleBar):
    class ButtonIcon(Enum):
        MinButtonIcon = 0
        MaxButtonIcon = 1
        RestoreButtonIcon = 2
        CloseButtonIcon = 3

    def __init__(self, parent=None):
        super().__init__(parent)
        self.layout = QHBoxLayout(self)
        self.icon = QIcon()
        self.title = ""
        self.minButton = QPushButton(self)
        self.maxButton = QPushButton(self)
        self.closeButton = QPushButton(self)
        self.flag = False
        self.standardIconMap = {}
        self.layout.setContentsMargins(0, 0, 0, 0)
        item = QSpacerItem(20, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        item1 = QSpacerItem(20, 20, QSizePolicy.Policy.Expanding,QSizePolicy.Policy.Expanding)
        self.standardIconMap[self.ButtonIcon.MinButtonIcon] = self.style().standardIcon(
            QStyle.StandardPixmap.SP_TitleBarMinButton)
        self.standardIconMap[self.ButtonIcon.MaxButtonIcon] = self.style().standardIcon(
            QStyle.StandardPixmap.SP_TitleBarMaxButton)
        self.standardIconMap[self.ButtonIcon.RestoreButtonIcon] = self.style().standardIcon(
            QStyle.StandardPixmap.SP_TitleBarNormalButton)
        self.standardIconMap[self.ButtonIcon.CloseButtonIcon] = self.style().standardIcon(
            QStyle.StandardPixmap.SP_TitleBarCloseButton)
        self.minButton.setFixedSize(25,25)
        self.minButton.setIcon(self.standardIconMap[self.ButtonIcon.MinButtonIcon])
        self.minButton.setCursor(QCursor.CursorShape.PointingHandCursor)


    def setTitleIcon(self, icon):
        if isinstance(icon, QIcon):
            self.icon = icon
        elif isinstance(icon, QPixmap):
            self.icon = QIcon(icon)

    def setTitleText(self, text):
        self.title = text

    def getStandardIconMap(self):
        return self.standardIconMap

    def setMinButtonIcon(self, icon):
        if isinstance(icon, QIcon):
            self.standardIconMap[self.ButtonIcon.MinButtonIcon] = icon
        elif isinstance(icon, str):
            self.standardIconMap[self.ButtonIcon.MinButtonIcon] = QIcon(icon)

    def setMaxButtonIcon(self, icon):
        if isinstance(icon, QIcon):
            self.standardIconMap[self.ButtonIcon.MaxButtonIcon] = icon
        elif isinstance(icon, str):
            self.standardIconMap[self.ButtonIcon.MaxButtonIcon] = QIcon(icon)

    def setRestoreButtonIcon(self, icon):
        if isinstance(icon, QIcon):
            self.standardIconMap[self.ButtonIcon.RestoreButtonIcon] = icon
        elif isinstance(icon, str):
            self.standardIconMap[self.ButtonIcon.RestoreButtonIcon] = QIcon(icon)

    def setCloseButtonIcon(self, icon):
        if isinstance(icon, QIcon):
            self.standardIconMap[self.ButtonIcon.CloseButtonIcon] = icon
        elif isinstance(icon, str):
            self.standardIconMap[self.ButtonIcon.CloseButtonIcon] = QIcon(icon)

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.drawPixmap(0, 0, self.icon.pixmap(16, 16))

    def closeButtonClick(self):
        self.closeButtonClicked.emit()

    def minButtonClick(self):
        self.minButtonClicked.emit()

    def mouseDoubleClickEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.closeButtonClick()

    def focusOutEvent(self, event):
        pass

    @Slot()
    def updateIcon(self):
        pass


class LWidget(QWidget):
    windowStateChanged = Signal()
    systemSettingsChanged = Signal()

    class Info:
        def __init__(self):
            self.edgeSize = 10
            self.radius = 10
            self.borderColor = Qt.GlobalColor.gray
            self.borderSize = 2
            self.backgroundColor = Qt.GlobalColor.white
            self.backgroundPixmap = QPixmap()
            self.splitLineColor = Qt.GlobalColor.gray

    def __init__(self, titleBar, mainWidget, parent=None):
        super().__init__(parent)
        self._titleBar = titleBar
        self._menuBar = None
        self._mainWidget = mainWidget
        self._statusBar = None
        self._layout = QVBoxLayout(self)
        self._info = self.Info()
        self.loadLayout()

    def loadLayout(self):
        self._layout.addWidget(self._titleBar)
        self._layout.addWidget(self._mainWidget)
        self._layout.addStretch(1)
        if self._statusBar:
            self._layout.addWidget(self._statusBar)

    def showCustomNormal(self):
        pass

    def setTitleBar(self, titleBar):
        self._titleBar = titleBar

    def setMenuBar(self, menuBar):
        self._menuBar = menuBar

    def setMainWidget(self, mainWidget):
        self._mainWidget = mainWidget

    def setStatusBar(self, statusBar):
        self._statusBar = statusBar

    def setInfo(self, info):
        self._info = info

    def info(self):
        return self._info

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setPen(QColor(self._info.borderColor))
        painter.drawRect(self.rect())

    def nativeEvent(self, eventType, message):
        return False

    def eventFilter(self, obj, event):
        return False

    def focusOutEvent(self, event):
        pass
