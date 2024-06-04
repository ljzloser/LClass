from enum import Enum
from PySide6.QtCore import Signal, QPointF, QRectF, Qt, QSize, QPropertyAnimation, Property
from PySide6.QtGui import QPainter, QPixmap, QPainterPath, QFontMetrics, QColor
from PySide6.QtWidgets import QPushButton, QAbstractButton


class LPixmapButton(QPushButton):
    def __init__(self, parent=None):
        super().__init__(parent)
        self._pixmap = QPixmap()
        self._drawPixmap = QPixmap()

    def setPixmap(self, pixmap: QPixmap):
        self._pixmap = pixmap
        self._drawPixmap = pixmap
        mask = self._drawPixmap.createMaskFromColor(Qt.GlobalColor.transparent)
        self.setFixedSize(self._drawPixmap.size())
        self.setMask(mask)

    def pixmap(self):
        return self._pixmap

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        painter.drawPixmap(0, 0, self._drawPixmap)

    def mousePressEvent(self, event):
        if not self.mask().contains(event.pos()):
            event.ignore()
        else:
            super().mousePressEvent(event)

    def enterEvent(self, event):
        super().enterEvent(event)
        self._drawPixmap = self.pixmap().scaled(self.pixmap().size() * 1.05, Qt.AspectRatioMode.ASpIgnoreAspectRatio,
                                                Qt.TransformationMode.SmoothTransformation)
        mask = self._drawPixmap.createMaskFromColor(Qt.GlobalColor.transparent)
        self.setFixedSize(self._drawPixmap.size())
        self.setMask(mask)

    def leaveEvent(self, event):
        super().leaveEvent(event)
        self.setPixmap(self.pixmap())


class LSwitchButton(QAbstractButton):
    stateChanged = Signal(object, object)

    class State(Enum):
        Off = 0
        On = 1

    class StateInfo:
        def __init__(self, state=None):
            if state is None:
                self.text = ""
                self.buttonPixmap = QPixmap()
                self.backgroundColor = QColor()
                self.textColor = QColor()
                self.circlePixmap = QPixmap()
            else:
                if state == LSwitchButton.State.On:
                    self.text = "On"
                    self.buttonPixmap = QPixmap()
                    self.backgroundColor = Qt.GlobalColor.white
                    self.textColor = Qt.GlobalColor.black
                else:
                    self.text = "Off"
                    self.buttonPixmap = QPixmap()
                    self.backgroundColor = Qt.GlobalColor.black
                    self.textColor = Qt.GlobalColor.white

        def reSize(self, radius):
            if self.buttonPixmap.isNull():
                self.circlePixmap = QPixmap()
            else:
                size = int(radius * 2)
                if self.circlePixmap.width() == size and self.circlePixmap.height() == size and not self.circlePixmap.isNull():
                    return
                pixmap = QPixmap(size, size)
                scaled = self.buttonPixmap.scaled(pixmap.size(), Qt.AspectRatioMode.IgnoreAspectRatio,
                                                  Qt.TransformationMode.SmoothTransformation)
                pixmap.fill(Qt.GlobalColor.transparent)
                painter = QPainter(pixmap)
                painter.setRenderHint(QPainter.RenderHint.Antialiasing)
                path = QPainterPath()
                path.addEllipse(0, 0, size, size)
                painter.setClipPath(path)
                painter.drawPixmap(0, 0, scaled)
                self.circlePixmap = pixmap

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setMouseTracking(True)
        self.setCheckable(True)
        self.clicked.connect(self.changeState)
        self._position = QPointF(0, 0)
        self._left = QPointF(0, 0)
        self._right = QPointF(0, 0)
        self._radius = 0
        self._buttonRect = QRectF(0, 0, 0, 0)
        self._isOpenAnimation = True
        self._animation = QPropertyAnimation(self, b'position')
        self._animation.setDuration(100)
        self._animation.finished.connect(self.updateValueRange)
        self._stateInfoMap = {self.State.Off: self.StateInfo(self.State.Off),
                              self.State.On: self.StateInfo(self.State.On)}

    def setStateInfo(self, state, stateInfo):
        self._stateInfoMap[state] = stateInfo

    def stateInfo(self, state):
        return self._stateInfoMap[state]

    def setState(self, state):
        self.setChecked(state == self.State.On)

    @Property(State, fset=setState)
    def state(self):
        return self.State.On if self.isChecked() else self.State.Off

    def setAnimationDuration(self, duration):
        self._animation.setDuration(duration)

    def animationDuration(self):
        return self._animation.duration()

    def setOpenAnimation(self, isOpenAnimation):
        self._isOpenAnimation = isOpenAnimation

    def isOpenAnimation(self):
        return self._isOpenAnimation

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        stateInfo = self._stateInfoMap[self.state]
        stateInfo.reSize(self._radius)
        isAnimationRunning = self._animation.state() == QPropertyAnimation.State.Running
        radius = self.calRadius(15) * 1.7

        # Draw background
        painter.setPen(Qt.NoPen)
        painter.setBrush(stateInfo.backgroundColor)
        painter.drawRoundedRect(self._buttonRect, radius, radius)

        # If animation is running, draw the background of the area not moved by the animation
        if isAnimationRunning:
            width = self._buttonRect.bottomRight().x() - self._position.x() if self.state == self.State.On else self._position.x() - self._buttonRect.topLeft().x()
            painter.setBrush(stateInfo.textColor)
            rect = QRectF(0, self._buttonRect.top(), width, self._buttonRect.height())
            if self.state == self.State.On:
                rect.moveLeft(self._position.x())
            else:
                rect.moveRight(self._position.x())
            painter.drawRoundedRect(rect, radius, radius)

        # Draw button
        painter.setPen(stateInfo.textColor)
        if stateInfo.circlePixmap.isNull():
            painter.setBrush(Qt.GlobalColor.transparent if isAnimationRunning else stateInfo.textColor)
            painter.drawEllipse(self._position, self._radius, self._radius)
        else:
            pixmapRect = stateInfo.circlePixmap.rect()
            pixmapRect.moveCenter(self._position.toPoint())
            painter.drawPixmap(pixmapRect, stateInfo.circlePixmap)

        # Draw text
        fontMetrics = QFontMetrics(self.font())
        textRect = fontMetrics.boundingRect(stateInfo.text)
        textRect.moveCenter(self._buttonRect.center().toPoint())
        painter.drawText(textRect, Qt.AlignmentFlag.AlignCenter, stateInfo.text)

        # Draw border
        painter.setPen(stateInfo.textColor)
        painter.setBrush(Qt.BrushStyle.NoBrush)
        painter.drawRoundedRect(self._buttonRect, radius, radius)

        # If disabled, draw mask
        if not self.isEnabled():
            painter.setBrush(QColor(128, 128, 128, 128))
            painter.setPen(Qt.PenStyle.NoPen)
            painter.drawRoundedRect(self._buttonRect, radius, radius)

    def updateValueRange(self):
        if self.state == self.State.On:
            self._animation.setStartValue(self._right)
            self._animation.setEndValue(self._left)
        else:
            self._animation.setStartValue(self._left)
            self._animation.setEndValue(self._right)

    def resizeEvent(self, event):
        super().resizeEvent(event)
        self.setRadius(self.calRadius(12))
        self._left = QPointF(self._radius + 5, self.height() / 2.0)
        self._right = QPointF(self.width() - self._radius - 5, self.height() / 2.0)
        self._buttonRect = QRectF(self.rect().x() + 2, self.rect().y() + 2, self.rect().width() - 4,
                                  self.rect().height() - 4)
        self.updateValueRange()
        self.setPosition(self._animation.startValue())

    def sizeHint(self):
        return QSize(60, 23)

    def enterEvent(self, event):
        super().enterEvent(event)

    def leaveEvent(self, event):
        super().leaveEvent(event)
        if self.isEnabled():
            self.setRadius(self.calRadius(12))

    def hitButton(self, pos):
        return self._buttonRect.contains(pos)

    def mouseMoveEvent(self, event):
        super().mouseMoveEvent(event)
        if self.isEnabled():
            if self._buttonRect.contains(event.position()):
                self.setRadius(self.calRadius(10))
            else:
                self.setRadius(self.calRadius(12))

    def setRadius(self, radius):
        self._radius = radius
        self.update()

    def calRadius(self, num):
        return min((self.width() - num) / 2.0, (self.height() - num) / 2.0)

    def changeState(self, checked):
        state = self.State.On if checked else self.State.Off
        self.stateChanged.emit(state, self._stateInfoMap[state])
        if self._isOpenAnimation:
            self._animation.start()
        else:
            self.setPosition(self._right if state == self.State.On else self._left)

    def setPosition(self, newValue):
        self._position = newValue
        self.update()

    @Property(QPointF, fset=setPosition)
    def position(self):
        return self._position
