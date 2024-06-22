import sys

from PySide6.QtWidgets import *
from LWidget import LMultiComboBox
from demo import DemoWidget

if __name__ == '__main__':
    app = QApplication(sys.argv)
    demo = DemoWidget()
    demo.show()
    sys.exit(app.exec())
