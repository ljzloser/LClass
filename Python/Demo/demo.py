from PySide6.QtWidgets import QMainWindow, QHBoxLayout, QListWidget, QListWidgetItem,QWidget

from LWidget.LComboBox import *


class DemoWidget(QMainWindow):
    def __init__(self):
        super().__init__()
        widget = QWidget()
        self._layout = QHBoxLayout()
        lists = ["LMultiComboBox(多选下拉列表)",
                 "LCompleteComboBox(补全下拉列表)"
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
                comboBox.addItems({"APPLE": False, "ORANGE": False, "BANANA": False})
                widget = comboBox
            case "LCompleteComboBox(补全下拉列表)":
                comboBox = LCompleteComboBox()
                comboBox.addItems(["APPLE", "ORANGE", "BANANA"])
                widget = comboBox
            case _:
                pass
        if widget is not None:
            self._widget.deleteLater()
            self._widget = widget
            self._layout.addWidget(self._widget, 1)
