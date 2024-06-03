from PySide6.QtWidgets import QMainWindow, QHBoxLayout, QListWidget, QListWidgetItem

from LWidget.LComboBox import *


class DemoWidget(QMainWindow):
    def __init__(self):
        super().__init__()
        widget = QWidget()
        self._layout = QHBoxLayout()
        lists = ["LMultiComboBox(多选下拉列表)",
                 ]
        self._listWidget = QListWidget()
        self._listWidget.addItems(lists)
        self._listWidget.setFixedWidth(300)
        self._widget = QWidget()
        self._layout.addWidget(self._listWidget)
        self._layout.addWidget(self._widget)
        widget.setLayout(self._layout)
        self.setCentralWidget(widget)
        self._listWidget.itemDoubleClicked.connect(self.onItemDoubleClicked)

    def onItemDoubleClicked(self, item: QListWidgetItem):
        text = item.text()
        widget: QWidget = None
        match text:
            case "LMultiComboBox(多选下拉列表)":
                comboBox = LMultiComboBox()
                comboBox.addItems({"APPLE": True, "ORANGE": True, "BANANA": True})
                widget = comboBox
            case _:
                pass
        if widget is not None:
            self._widget.deleteLater()
            self._widget = widget
            self._layout.addWidget(self._widget, 1)
