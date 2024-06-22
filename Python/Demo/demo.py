from PySide6.QtWidgets import QMainWindow, QHBoxLayout, QListWidget, QListWidgetItem,QWidget

from LWidget import *


class DemoWidget(QMainWindow):
    def __init__(self):
        super().__init__()
        widget = QWidget()
        self._layout = QHBoxLayout()
        lists = ["LMultiComboBox(多选下拉列表)",
                 "LCompleteComboBox(补全下拉列表)",
                 "LSwitchButton(开关按钮)",
                 "LTextEdit(文本输入框)",
                 "LFileLineEdit(文件路径输入框)",
                 "LFocusSelectLineEdit(聚焦即选中输入框)",
                 "LHostAddressLineEdit(IP地址输入框)",
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
            case "LSwitchButton(开关按钮)":
                switchButton = LSwitchButton()
                widget = switchButton
            case "LTextEdit(文本输入框)":
                textEdit = LTextEdit(self)
                textEdit.setPlainText("Hello World")
                textEdit.setPlainText("""{
    name:asdasd,
    type:person,
    email:13123581@qq.com,
    type:person,
}""")
                textEdit.setLineNumberAreaVisible(True)
                widget = textEdit
            case "LFileLineEdit(文件路径输入框)":
                fileLineEdit = LFileLineEdit(self)
                info = LFileLineEdit.Info(mode = QFileDialog.FileMode.ExistingFiles,filters=["*.txt", "*.py"])
                fileLineEdit.setInfo(info)
                widget = fileLineEdit
            case "LFocusSelectLineEdit(聚焦即选中输入框)":
                lineEdit = LFocusSelectLineEdit()
                lineEdit.setText("Hello World")
                widget = lineEdit
            case "LHostAddressLineEdit(IP地址输入框)":
                lineEdit = LHostAddressEdit()
                lineEdit.setHostAddress(QHostAddress("127.0.0.1"))
                widget = lineEdit
            case _:
                pass
        if widget is not None:
            self._widget.deleteLater()
            self._widget = widget
            self._layout.addWidget(self._widget, 1)
