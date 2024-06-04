from LWidget.LButton import LSwitchButton
from LCore import *
from PySide6.QtWidgets import *
from PySide6.QtCore import *
from PySide6.QtGui import *
from typing import *
import html


class LLineNumberArea(QWidget):
    pass


class LTextEdit(QPlainTextEdit):
    def searchToolWidget(self):
        pass


class FindItemInfo(object):
    def __init__(self, lineNumber: int = 0, rowText: str = "", findBegin: int = 0, findEnd: int = 0):
        self.lineNumber = lineNumber
        self.rowText = rowText
        self.findBegin = findBegin
        self.findEnd = findEnd


class LFindItemDialogDelegate(QStyledItemDelegate):

    def paint(self, painter: QPainter, option: QStyleOptionViewItem, index: QModelIndex) -> None:
        data = index.data(Qt.ItemDataRole.UserRole + 1)
        if data is not None:
            _map: dict = data
            content: str = _map["内容"]
            if content:
                _from: int = _map["开始位置"]
                _to: int = _map["结束位置"]
                left: str = html.escape(content[:_from])
                mid: str = html.escape(content[_from:_to])
                right: str = html.escape(content[_to:])
                highlightText: str = (f"""<span>{LFunc.replaceSpaceToHtml(left)}</span>
                <span style='background-color: yellow;'>{LFunc.replaceSpaceToHtml(mid)}</span>
                <span>{LFunc.replaceSpaceToHtml(right)}</span>""")
                document = QTextDocument()
                document.setHtml(highlightText)
                option.widget.style().drawControl(QStyle.ControlElement.CE_ItemViewItem, option, painter, option.widget)
                painter.save()
                painter.translate(option.rect.topLeft())
                document.drawContents(painter)
                painter.restore()
                return
        super().paint(painter, option, index)


class LFindItemDialog(QDialog):
    findItem = Signal(FindItemInfo)

    def __init__(self, parent: QWidget = None):
        super().__init__(parent)
        self._tableView = QTableView(self)
        self._label = QLabel(self)
        layout = QVBoxLayout()
        layout.addWidget(self._tableView)
        layout.addWidget(self._label)
        self.setLayout(layout)
        self.setWindowIcon(QIcon(":/res/res/searchResult.png"))
        self.setWindowTitle("搜素结果")
        self._tableView.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.ResizeToContents)
        self._tableView.horizontalHeader().setStretchLastSection(True)
        self._tableView.setEditTriggers(QAbstractItemView.EditTrigger.NoEditTriggers)
        # 整行选中
        self._tableView.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)
        self._tableView.setItemDelegate(LFindItemDialogDelegate(self))
        self._tableView.doubleClicked.connect(self._onTableViewDoubleClicked)

    def _onTableViewDoubleClicked(self, index: QModelIndex):
        info = FindItemInfo()
        row = self._tableView.currentIndex().row()
        map: dict = self._tableView.model().index(row, 3).data(Qt.ItemDataRole.UserRole + 1)
        info.lineNumber = map["行号"]
        info.rowText = map["内容"]
        info.findBegin = map["开始位置"]
        info.findEnd = map["结束位置"]
        self.findItem.emit(info)

    def setItemInfos(self, keys: List[str], maps: List[dict]):
        model = QStandardItemModel(self._tableView)
        model.setHorizontalHeaderLabels(keys)
        for _dict in maps:
            _list = []
            for key in keys:
                item = QStandardItem(_dict[key])
                if key == "内容":
                    item.setData(_dict, Qt.ItemDataRole.UserRole + 1)
                else:
                    item.setText(str(_dict[key] + 1))
                _list.append(item)
            model.appendRow(_list)
        self._tableView.setModel(model)
        self._label.setText(f"共搜索到{len(maps)}条")


class LTextEditKeyPress(QObject):
    def eventFilter(self, watched: QObject, event: QEvent) -> bool:
        if event.type() == QEvent.Type.KeyPress:
            keyEvent: QKeyEvent = event
            if keyEvent.key() in [Qt.Key.Key_Return, Qt.Key.Key_Enter, Qt.Key.Key_Tab]:
                if isinstance(watched, LTextEdit):
                    textEdit: LTextEdit = watched
                    if textEdit.searchToolWidget().isHidden() or textEdit.hasFocus():
                        return super().eventFilter(watched, event)
                    else:
                        return textEdit.searchToolWidget().eventFilter(textEdit.searchToolWidget(), event)
            else:
                return super().eventFilter(watched, event)
            return True
        else:
            return super().eventFilter(watched, event)


class NoNewlineValidator(QValidator):
    def validate(self, arg__1: str, arg__2: int) -> QValidator.State:
        if "\n" in arg__1 or "\r" in arg__1:
            return QValidator.State.Invalid
        return QValidator.State.Acceptable


class LSearchHighlighter(QSyntaxHighlighter):
    def __init__(self, parent=None):
        super().__init__(parent)
        self._searchBrush = QBrush(QColor(255, 0, 0))
        self._re = QRegularExpression()
        self._selectCursor = QTextCursor()
        self._selectBrush = QBrush(QColor(0, 255, 0))
        self._nextCursor = QTextCursor()
        self._nextBrush = QBrush(QColor(0, 0, 255))
        self._findGround = QBrush(QColor(255, 255, 0))
        self._isHighlight = False

    def setSearchBrush(self, searchBrush: QBrush):
        self._searchBrush = searchBrush

    def setRe(self, re: QRegularExpression):
        self._re = re

    def setSelectCursor(self, selectCursor: QTextCursor):
        self._selectCursor = selectCursor

    def setSelectBrush(self, selectBrush: QBrush):
        self._selectBrush = selectBrush

    def setNextCursor(self, nextCursor: QTextCursor):
        self._nextCursor = nextCursor

    def setNextBrush(self, nextBrush: QBrush):
        self._nextBrush = nextBrush

    def serFindGround(self, findGround: QBrush):
        self._findGround = findGround

    def setIsHighlight(self, isHighlight: bool):
        self._isHighlight = isHighlight

    def highlightBlock(self, text: str):
        pos = self.currentBlock().position()
        if not self._isHighlight:
            return
        fmt3 = QTextCharFormat()
        fmt3.setBackground(self._selectBrush)

        if not self._selectCursor.isNull():
            self.setFormat(max(0, self._selectCursor.selectionStart() - pos),
                           min(self.currentBlock().length(), self._selectCursor.selectionEnd() - pos),
                           fmt3)

        i: QRegularExpressionMatchIterator = self._re.globalMatch(text)
        while i.hasNext():
            fmt = QTextCharFormat()
            fmt.setBackground(self._findGround)
            match: QRegularExpressionMatch = i.next()
            start = min(self._selectCursor.selectionStart(), self._selectCursor.selectionEnd())
            end = max(self._selectCursor.selectionStart(), self._selectCursor.selectionEnd())

            if not self._nextCursor.isNull() and match.capturedStart() + pos == self._nextCursor.selectionStart() and match.capturedEnd() + pos == self._nextCursor.selectionEnd():
                fmt.setForeground(self._nextBrush)
            else:
                fmt.setForeground(self._searchBrush)

            if self._selectCursor.isNull() or (match.capturedStart() + pos < start and match.capturedEnd() + pos < start) or (match.capturedStart() + pos > end and match.capturedEnd() + pos > end):
                self.setFormat(match.capturedStart() + pos, match.capturedLength(), fmt)
            else:
                if start > match.capturedStart() + pos:
                    self.setFormat(start, match.capturedEnd() + pos - start, fmt)
                if end < match.capturedEnd() + pos:
                    self.setFormat(end, match.capturedEnd() + pos - end, fmt)
                fmt.setBackground(self._selectBrush)
                self.setFormat(
                    max(match.capturedStart(), start - pos),
                    min(match.capturedEnd() + pos, end) - max(match.capturedStart() + pos, start),
                    fmt
                )


class LTextEditToolWidget(QFrame):
    search = Signal(str, QTextDocument.FindFlag, bool)
    searchMove = Signal(str, QTextDocument.FindFlag, bool)
    replace = Signal(str, str, QTextDocument.FindFlag, bool)
    replaceAll = Signal(str, str, QTextDocument.FindFlag, bool)
    seeFindResult = Signal(str, QTextDocument.FindFlag, bool)

    def __init__(self, parent=None):
        super().__init__(parent)
        self._searchLineEdit = QLineEdit(self)
        self._searchButton = QPushButton("查找", self)
        self._comboBox = QComboBox(self)
        self._caseSensitiveButton = LSwitchButton(self)
        self._wholeWordButton = LSwitchButton(self)
        self._regExpButton = LSwitchButton(self)
        self._button = QPushButton(self)
        self._seeButton = QPushButton(self)
        self._replaceNextButton = QPushButton("替换下一个", self)
        self._replaceAllButton = QPushButton("全部替换", self)
        self._replaceLineEdit = QLineEdit(self)
        self.init_ui()

    def init_ui(self):
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        self.setFrameShape(QFrame.Shape.WinPanel)

        self._searchLineEdit.setPlaceholderText("查找...")
        self._comboBox.addItems(["向下查找", "向上查找"])

        rowLayout1 = QHBoxLayout()
        rowLayout1.addWidget(self._searchLineEdit)
        rowLayout1.addWidget(self._comboBox)
        rowLayout1.addWidget(self._searchButton)

        rowLayout3 = QHBoxLayout()
        rowLayout3.addWidget(QLabel("大小写", self))
        rowLayout3.addWidget(self._caseSensitiveButton)
        rowLayout3.addWidget(QLabel("全字", self))
        rowLayout3.addWidget(self._wholeWordButton)
        rowLayout3.addWidget(QLabel("正则", self))
        rowLayout3.addWidget(self._regExpButton)

        rowLayout2 = QHBoxLayout()
        self._replaceLineEdit.setPlaceholderText("替换...")
        rowLayout2.addWidget(self._replaceLineEdit)
        rowLayout2.addWidget(self._replaceNextButton)
        rowLayout2.addWidget(self._replaceAllButton)

        layout.addLayout(rowLayout1)
        layout.addLayout(rowLayout2)
        layout.addLayout(rowLayout3)

        mainLayout = QHBoxLayout()
        leftLayout = QVBoxLayout()
        self._button.setToolTip("在查找和替换模式之间切换")
        self._button.setCheckable(True)
        self._button.setIcon(QIcon(":/res/res/down.png"))
        self._button.setFixedSize(20, 20)
        self._seeButton.setIcon(QIcon(":/res/res/searchResult.png"))
        self._seeButton.setToolTip("查看搜索结果")
        self._seeButton.setFixedSize(20, 20)
        leftLayout.addWidget(self._button)
        leftLayout.addSpacerItem(QSpacerItem(20, 20, QSizePolicy.Expanding, QSizePolicy.Expanding))
        leftLayout.addWidget(self._seeButton)
        mainLayout.addLayout(leftLayout)
        mainLayout.addLayout(layout)
        mainLayout.setContentsMargins(0, 0, 0, 0)
        self.setLayout(mainLayout)
        self.setFixedSize(300, 50)
        self._searchLineEdit.setValidator(NoNewlineValidator())
        self._replaceLineEdit.setValidator(NoNewlineValidator())
        self._replaceLineEdit.hide()
        self._replaceNextButton.hide()
        self._replaceAllButton.hide()
        self.setFixedHeight(50)
        self._button.setChecked(False)
        self._button.setIcon(QIcon(":/res/res/down.png"))
        self._button.toggled.connect(self.on_button_toggled)
        self._searchLineEdit.textChanged.connect(self.on_search_text_changed)
        self._searchLineEdit.returnPressed.connect(self.on_return_pressed)
        self._searchButton.clicked.connect(self.on_search_button_clicked)
        self._regExpButton.stateChanged.connect(self.on_reg_exp_state_changed)
        self._caseSensitiveButton.stateChanged.connect(self.on_case_sensitive_state_changed)
        self._wholeWordButton.stateChanged.connect(self.on_whole_word_state_changed)
        self._replaceNextButton.clicked.connect(self.on_replace_next_button_clicked)
        self._replaceAllButton.clicked.connect(self.on_replace_all_button_clicked)
        self._seeButton.clicked.connect(self.on_see_button_clicked)

    def on_button_toggled(self, checked):
        if checked:
            self._button.setIcon(QIcon(":/res/res/up.png"))
            self._replaceLineEdit.show()
            self._replaceNextButton.show()
            self._replaceAllButton.show()
            self.setFixedHeight(75)
        else:
            self._button.setIcon(QIcon(":/res/res/down.png"))
            self._replaceLineEdit.hide()
            self._replaceNextButton.hide()
            self._replaceAllButton.hide()
            self.setFixedHeight(50)

    def on_search_text_changed(self):
        self.search.emit(self._searchLineEdit.text(), self.createFlags(), self._regExpButton.isChecked())

    def on_return_pressed(self):
        self.searchMove.emit(self._searchLineEdit.text(), self.createFlags(), self._regExpButton.isChecked())

    def on_search_button_clicked(self):
        self.searchMove.emit(self._searchLineEdit.text(), self.createFlags(), self._regExpButton.isChecked())

    def on_reg_exp_state_changed(self):
        self.search.emit(self._searchLineEdit.text(), self.createFlags(), self._regExpButton.isChecked())

    def on_case_sensitive_state_changed(self):
        self.search.emit(self._searchLineEdit.text(), self.createFlags(), self._regExpButton.isChecked())

    def on_whole_word_state_changed(self):
        self.search.emit(self._searchLineEdit.text(), self.createFlags(), self._regExpButton.isChecked())

    def on_replace_next_button_clicked(self):
        self.replace.emit(self._searchLineEdit.text(), self._replaceLineEdit.text(), self.createFlags(),
                          self._regExpButton.isChecked())

    def on_replace_all_button_clicked(self):
        self.replaceAll.emit(self._searchLineEdit.text(), self._replaceLineEdit.text(), self.createFlags(),
                             self._regExpButton.isChecked())

    def on_see_button_clicked(self):
        self.seeFindResult.emit(self._searchLineEdit.text(), self.createFlags(), self._regExpButton.isChecked())

    def setSearchText(self, text):
        self._searchLineEdit.setText(text)
        self.search.emit(self._searchLineEdit.text(), self.createFlags(), self._regExpButton.isChecked())

    def init(self):
        self._replaceLineEdit.clear()
        self._searchLineEdit.clear()
        self._button.setChecked(False)
        self._button.setIcon(QIcon(":/res/res/down.png"))
        self._replaceLineEdit.hide()
        self._replaceNextButton.hide()
        self._replaceAllButton.hide()
        self.setFixedHeight(50)

    def focusOutEvent(self, event: QFocusEvent):
        super().focusOutEvent(event)
        self.search.emit(self._searchLineEdit.text(), self.createFlags(),False)

    def createFlags(self):
        findFlags = QTextDocument.FindFlag(0)
        if self._comboBox.currentIndex() == 0:
            findFlags &= ~QTextDocument.FindFlag.FindBackward
        else:
            findFlags |= QTextDocument.FindFlag.FindBackward
        if self._caseSensitiveButton.isChecked():
            findFlags |= QTextDocument.FindFlag.FindCaseSensitively
        else:
            findFlags &= ~QTextDocument.FindFlag.FindCaseSensitively
        if self._wholeWordButton.isChecked():
            findFlags |= QTextDocument.FindFlag.FindWholeWords
        else:
            findFlags &= ~QTextDocument.FindFlag.FindWholeWords
        return findFlags

    def focusInEvent(self, event: QFocusEvent):
        super().focusInEvent(event)
        self.search.emit(self._searchLineEdit.text(), self.createFlags(), False)

    def showEvent(self, event: QShowEvent):
        super().showEvent(event)
        self.setFocus()
        self._searchLineEdit.setFocus()


class LTextEdit(QPlainTextEdit):
    notFind = Signal()

    class ColorInfo:
        def __init__(self, allFindBrush: QBrush = QColor(Qt.GlobalColor.darkGreen),
                     nextFindBrush: QBrush = QColor(Qt.GlobalColor.red),
                     rangeFindBrush: QBrush = QColor(135, 206, 235),
                     findGroundBrush: QBrush = QColor(Qt.GlobalColor.yellow)
                     ):
            self.allFindBrush = allFindBrush
            self.nextFindBrush = nextFindBrush
            self.rangeFindBrush = rangeFindBrush
            self.findGroundBrush = findGroundBrush

    def __init__(self, parent=None):
        super().__init__(parent)
        self._searchToolWidget = LTextEditToolWidget(self)
        self._selectCursor = QTextCursor()
        self._findData: Any = None
        self._findFlags = QTextDocument.FindFlag(0)
        self._isRegExp = False
        self._lastCursor = QTextCursor()
        self._colorInfo = LTextEdit.ColorInfo()
        self._hashText = ""
        self._highlighter = LSearchHighlighter(self.document())
        self._dialog = LFindItemDialog(self)
        self._lineNumberArea = LLineNumberArea(self)
        self._isHighlightCurrentLine = False
        self.initUI()

    def initUI(self):
        self._searchToolWidget.hide()
        self.installEventFilter(LTextEditKeyPress(self))
        self._lineNumberArea.hide()
        self.updateLineNumberAreaWidth(0)
        self.blockCountChanged.connect(self.updateLineNumberAreaWidth)
        self.updateRequest.connect(self.updateLineNumberArea)
        self.cursorPositionChanged.connect(self.highlightCurrentLine)
        self._searchToolWidget.search.connect(self.search)
        self._searchToolWidget.searchMove.connect(self.searchMove)
        self._searchToolWidget.replace.connect(self.replace)
        self._searchToolWidget.replaceAll.connect(self.replaceAll)
        self._searchToolWidget.seeFindResult.connect(self.showFindResult)
        self.textChanged.connect(self.textChangedSlot)
        self._highlighter.setSelectBrush(self._colorInfo.rangeFindBrush)
        self._highlighter.setSearchBrush(self._colorInfo.allFindBrush)
        self._highlighter.setNextBrush(self._colorInfo.nextFindBrush)
        self._highlighter.serFindGround(self._colorInfo.findGroundBrush)
        metrics = QFontMetrics(self.font())
        tabStopWidth = metrics.horizontalAdvance("    ")
        option: QTextOption = self.document().defaultTextOption()
        option.setTabStopDistance(tabStopWidth)
        self.document().setDefaultTextOption(option)
        font: QFont = self.font()
        font.setPointSize(12)
        self.setFont(font)

    def searchToolWidget(self) -> LTextEditToolWidget:
        return self._searchToolWidget

    def setColorInfo(self, colorInfo: ColorInfo):
        self._colorInfo = colorInfo

    def copySelectCursor(self, cursor: QTextCursor):
        if cursor.hasSelection() and self._selectCursor.isNull():
            self._selectCursor = QTextCursor(self.document())
            self._highlighter.setSelectCursor(self._selectCursor)
        if not self._selectCursor.isNull():
            tmpCursor: QTextCursor = QTextCursor(self._selectCursor)
            tmpCursor.clearSelection()
            self.setTextCursor(tmpCursor)
        self._highlighter.rehighlight()

    def search(self, text: str, findFlags: QTextDocument.FindFlag, isRegExp: bool):
        self._isRegExp = isRegExp
        self._findFlags = findFlags
        self._lastCursor = QTextCursor()
        self._findData = text
        re: QRegularExpression = QRegularExpression(text)
        if not re.isValid() and isRegExp:
            re = QRegularExpression()
        self.textChanged.disconnect(self.textChangedSlot)
        self.blockSignals(True)
        self.setUpdatesEnabled(False)

        if not isRegExp:
            re = QRegularExpression(LFunc.escapeSpecialCharacters(re.pattern()))
            if findFlags & QTextDocument.FindFlag.FindWholeWords:
                re = QRegularExpression(f"\\b{re.pattern()}\\b")
        if not (findFlags & QTextDocument.FindFlag.FindCaseSensitively):
            re.setPatternOptions(QRegularExpression.PatternOption.CaseInsensitiveOption)
        self._highlighter.setRe(re);
        self.copySelectCursor(self.textCursor())
        self.blockSignals(False)
        self.setUpdatesEnabled(True)
        self.textChanged.connect(self.textChangedSlot)

    def searchMove(self, text: str, findFlags: QTextDocument.FindFlag, isRegExp: bool):
        if text is None or text == "":
            return
        re: QRegularExpression = QRegularExpression(text)
        if not re.isValid() and isRegExp:
            return
        self.textChanged.disconnect(self.textChangedSlot)
        cursor: QTextCursor = self.textCursor()
        document: QTextDocument = self.document()
        findCursor = QTextCursor()
        if not isRegExp:
            findCursor = document.find(text, cursor, findFlags)
        else:
            findCursor = document.find(re, cursor, findFlags)

        if findFlags & QTextDocument.FindFlag.FindBackward and not self._lastCursor.isNull():
            if not isRegExp:
                findCursor = document.find(text, self._lastCursor, findFlags)
            else:
                if not (findFlags & QTextDocument.FindFlag.FindCaseSensitively):
                    re.setPatternOptions(QRegularExpression.PatternOption.CaseInsensitiveOption)
                    findCursor = document.find(re, self._lastCursor, findFlags)
        self.copySelectCursor(self.textCursor())
        if not self._selectCursor.isNull():
            a = self._selectCursor.selectionStart()
            b = self._selectCursor.selectionEnd()
            _from = min(a, b)
            _to = max(a, b)
            pos = findCursor.position()
            if (pos < _from or pos > _to) or findCursor.isNull():
                self.notFind.emit()
                self.textChanged.connect(self.textChangedSlot)
                return
        self._lastCursor = findCursor
        self._highlighter.setNextCursor(findCursor)
        self._highlighter.rehighlight()
        findCursor.clearSelection()
        if not findCursor.isNull():
            self.setTextCursor(findCursor)
        self.textChanged.connect(self.textChangedSlot)

    def replace(self, text: str, replaceText: str, findFlags: QTextDocument.FindFlag, isRegExp: bool):
        if text == "":
            return
        re = QRegularExpression(text)
        if not re.isValid() and isRegExp:
            return
        cursor: QTextCursor = self.textCursor()
        document: QTextDocument = self.document()
        findCursor: QTextCursor = QTextCursor()

        if not isRegExp:
            findCursor = document.find(text, cursor if self._lastCursor.isNull() else self._lastCursor, findFlags)
        else:
            findCursor = document.find(re, cursor if self._lastCursor.isNull() else self._lastCursor, findFlags)

        self.copySelectCursor(self.textCursor())
        if not self._selectCursor.isNull():
            a = self._selectCursor.selectionStart()
            b = self._selectCursor.selectionEnd()
            _from = min(a, b)
            _to = max(a, b)
            pos = findCursor.position();
            if (pos < _from or pos > _to) or findCursor.isNull():
                self.notFind.emit()
                return
        self._lastCursor = findCursor
        if isRegExp:
            findText = findCursor.selectedText()
            newReplaceText = replaceText
            expression = QRegularExpression(text)
            if not findFlags & QTextDocument.FindFlag.FindCaseSensitively:
                expression.setPatternOptions(QRegularExpression.PatternOption.CaseInsensitiveOption)
            match: QRegularExpressionMatch = expression.match(findText)
            replaceRe: QRegularExpression = QRegularExpression("\\$([0-9]+)")
            it: QRegularExpressionMatchIterator = replaceRe.globalMatch(replaceText)
            while it.hasNext():
                thisMatch: QRegularExpressionMatch = it.next()
                number = thisMatch.captured(1)
                newString = match.captured(int(number))
                newReplaceText = newReplaceText.replace("$" + number, newString)
            findCursor.insertText(newReplaceText)
        else:
            findCursor.insertText(replaceText)
        if not findCursor.isNull():
            self.setTextCursor(findCursor)

    def replaceAll(self, text: str, replace_text: str, find_flags: QTextDocument.FindFlag, is_regexp: bool):
        if not text:
            return
        re = QRegularExpression(text)
        if is_regexp and not re.isValid():
            return

        self.textChanged.disconnect(self.textChangedSlot)
        self.setUpdatesEnabled(False)

        _from = 0
        self.copySelectCursor(self.textCursor())
        count = 0

        if not self._selectCursor.isNull():
            a = self._selectCursor.selectionStart()
            b = self._selectCursor.selectionEnd()
            _from = min(a, b)

        find_cursor = QTextCursor(self.document())
        find_cursor.setPosition(_from, QTextCursor.MoveAnchor)
        pos = -1
        find_cursor.beginEditBlock()

        while True:
            _from = 0
            _to = len(self.document().toPlainText())

            if not self._selectCursor.isNull():
                a = self._selectCursor.selectionStart()
                b = self._selectCursor.selectionEnd()
                _from = min(a, b)
                _to = max(a, b)

            if not is_regexp:
                tmp_cursor = self.document().find(text, find_cursor, find_flags)
            else:
                tmp_cursor = self.document().find(re, find_cursor, find_flags)

            if pos == tmp_cursor.position():
                break
            else:
                pos = tmp_cursor.position()

            if (pos < _from or pos > _to) or tmp_cursor.isNull():
                self.notFind.emit()
                break
            else:
                find_cursor.setPosition(tmp_cursor.selectionStart(), QTextCursor.MoveAnchor)
                find_cursor.setPosition(tmp_cursor.selectionEnd(), QTextCursor.KeepAnchor)

                if is_regexp:
                    find_text = find_cursor.selectedText()
                    new_replace_text = replace_text
                    expression = QRegularExpression(text)
                    if not find_flags & QTextDocument.FindCaseSensitively:
                        expression.setPatternOptions(QRegularExpression.CaseInsensitiveOption)
                    match = expression.match(find_text)
                    replace_re = QRegularExpression(r"\$([0-9]+)")
                    it = replace_re.globalMatch(replace_text)
                    while it.hasNext():
                        this_match = it.next()
                        number = this_match.captured(1)
                        new_string = match.captured(int(number))
                        new_replace_text = new_replace_text.replace(f"${number}", new_string)
                    find_cursor.insertText(new_replace_text)
                else:
                    find_cursor.insertText(replace_text)

                count += 1
                self.setTextCursor(find_cursor)

            if tmp_cursor.isNull():
                break

        find_cursor.endEditBlock()
        self.setUpdatesEnabled(True)
        self.textChanged.connect(self.textChangedSlot)
        self._highlighter.rehighlight()
        self.update()
        QMessageBox.information(self, "提示", f"共替换了 {count} 个")

    def showFindResult(self, text: str, find_flags: QTextDocument.FindFlags, is_regexp: bool):
        if not text:
            return
        re = QRegularExpression(text)
        if is_regexp and not re.isValid():
            return

        find_flags &= ~QTextDocument.FindFlag.FindBackward
        vself.textChanged.disconnect(self.textChangedSlot)
        self.setUpdatesEnabled(False)
        self.blockSignals(True)

        _from = 0
        self.copySelectCursor(self.textCursor())

        if not self._selectCursor.isNull():
            a = self._selectCursor.selectionStart()
            b = self._selectCursor.selectionEnd()
            _from = min(a, b)

        find_cursor = QTextCursor(self.document())
        find_cursor.setPosition(_from, QTextCursor.MoveMode.MoveAnchor)
        pos = -1
        _list: list[FindItemInfo] = []

        while True:
            _from = 0
            _to = len(self.document().toPlainText())

            if not self._selectCursor.isNull():
                a = self._selectCursor.selectionStart()
                b = self._selectCursor.selectionEnd()
                _from = min(a, b)
                _to = max(a, b)

            if not is_regexp:
                tmp_cursor = self.document().find(text, find_cursor, find_flags)
            else:
                tmp_cursor = self.document().find(re, find_cursor, find_flags)

            if pos == tmp_cursor.position():
                break
            else:
                pos = tmp_cursor.position()

            if (pos < _from or pos > _to) or tmp_cursor.isNull():
                self.notFind.emit()
                break
            else:
                find_cursor.setPosition(tmp_cursor.selectionStart(), QTextCursor.MoveMode.MoveAnchor)
                find_cursor.setPosition(tmp_cursor.selectionEnd(), QTextCursor.MoveMode.KeepAnchor)

                block = find_cursor.block()
                info = FindItemInfo()
                info.line_number = block.blockNumber()
                info.row_text = block.text()
                info.find_begin = find_cursor.selectionStart() - block.position()
                info.find_end = find_cursor.selectionEnd() - block.position()
                _list.append(info)
                self.setTextCursor(find_cursor)

        self.blockSignals(False)
        self.setUpdatesEnabled(True)
        cursor = self.textCursor()
        cursor.clearSelection()
        self.setTextCursor(cursor)
        self.textChanged.connect(self.textChangedSlot)

        if self._dialog:
            del self._dialog
            self._dialog = None

        self._dialog = LFindItemDialog(self)
        self._dialog.findItem.connect(self.findItemClicked)

        keys = ["行号", "开始位置", "结束位置", "内容"]
        maps = []

        for item in _list:
            map = {}
            map["行号"] = item.lineNumber
            map["内容"] = item.rowText
            map["开始位置"] = item.findBegin
            map["结束位置"] = item.findEnd
            maps.append(map)

        self._dialog.setItemInfos(keys, maps)
        self._dialog.resize(600, 200)
        self._dialog.show()

    def lineNumberAreaPaintEvent(self, event: QPaintEvent):
        painter = QPainter(self._lineNumberArea)
        painter.fillRect(event.rect(), Qt.GlobalColor.lightGray)
        block = self.firstVisibleBlock()
        blockNumber = block.blockNumber()
        top = round(self.blockBoundingGeometry(block).translated(self.contentOffset()).top())
        bottom = top + round(self.blockBoundingRect(block).height())
        while block.isValid() and top <= event.rect().bottom():
            if block.isVisible() and bottom >= event.rect().top():
                number = str(blockNumber + 1)
                painter.setPen(Qt.GlobalColor.black)
                painter.drawText(0, top, self._lineNumberArea.width(),
                                 self.fontMetrics().height(),
                                 Qt.AlignmentFlag.AlignRight, number)
            block = block.next()
            top = bottom
            bottom = top + round(self.blockBoundingRect(block).height())
            blockNumber += 1
        self.update()

    def lineNumberAreaWidth(self) -> int:
        digits = 1
        _max = max(1, int(self.blockCount()))
        while _max >= 10:
            _max /= 10
            digits += 1
        space = 3 + self.fontMetrics().horizontalAdvance('9') * digits
        return space

    def updateLineNumberAreaWidth(self, param):
        self.setViewportMargins(self.lineNumberAreaWidth(), 0, 0, 0)

    def highlightCurrentLine(self):
        if not self.isHighlightCurrentLine():
            return
        extraSelections: list[QTextEdit.ExtraSelection] = []
        if not self.isReadOnly():
            selection: QTextEdit.ExtraSelection = QTextEdit.ExtraSelection()
            lineColor = QColor(Qt.GlobalColor.gray).lighter(130)
            selection.format.setBackground(lineColor)
            selection.format.setProperty(QTextFormat.Property.FullWidthSelection, True)
            selection.cursor = self.textCursor()
            selection.cursor.clearSelection()
            extraSelections.append(selection)
        self.setExtraSelections(extraSelections)

    def updateLineNumberArea(self, rect, dy):
        if dy:
            self._lineNumberArea.scroll(0, dy)
        else:
            self._lineNumberArea.update(0, rect.y(), self._lineNumberArea.width(), rect.height())

        if rect.contains(self.viewport().rect()):
            self.updateLineNumberAreaWidth(0)

    def selectLineByPoint(self, pos: QPoint):
        cursor = self.cursorForPosition(pos)
        lineText = cursor.block().text()
        lineStart = cursor.block().position()
        lineEnd = lineStart + lineText.length()
        cursor.setPosition(lineStart)
        cursor.setPosition(lineEnd, QTextCursor.MoveMode.KeepAnchor)
        self.setTextCursor(cursor)

    def setLineNumberAreaVisible(self, visible: bool):
        if visible:
            self._lineNumberArea.show()
        else:
            self._lineNumberArea.hide()

    def isLineNumberAreaVisible(self) -> bool:
        return self._lineNumberArea.isHidden()

    def setHighlightCurrentLine(self, highlight: bool):
        self._isHighlightCurrentLine = highlight

    def isHighlightCurrentLine(self) -> bool:
        return self._isHighlightCurrentLine

    def keyPressEvent(self, event: QKeyEvent):
        super().keyPressEvent(event)
        if event.key() == Qt.Key.Key_F and event.modifiers() == Qt.KeyboardModifier.ControlModifier:
            if self._searchToolWidget.isHidden():
                self._searchToolWidget.init()
                self._searchToolWidget.show()
                text: str = self.textCursor().selectedText() if self._selectCursor.isNull() else self._selectCursor.selectedText()
                isLines = "\xE2\x80\xA9""" in text
                if not isLines:
                    self._searchToolWidget.setSearchText(text)
                self.copySelectCursor(self.textCursor())
                self._highlighter.setIsHighlight(True)
                self._highlighter.rehighlight()
            else:
                self._searchToolWidget.hide()
                self.setFocus()
                self._highlighter.setIsHighlight(False)
                self._highlighter.rehighlight()
                self._selectCursor = QTextCursor()
                if self._dialog:
                    self._dialog.deleteLater()
                    self._dialog = None

            rect = self._searchToolWidget.rect()
            right = 0 if self.verticalScrollBar().isHidden() else self.verticalScrollBar().width()
            rect.moveTopRight(self.rect().topRight())
            rect.moveRight(rect.right() - right - 1)
            self._searchToolWidget.setGeometry(rect)

    def resizeEvent(self, event: QResizeEvent):
        super().resizeEvent(event)
        rect = self._searchToolWidget.rect()
        right = 0 if self.verticalScrollBar().isHidden() else self.verticalScrollBar().width()
        rect.moveTopRight(self.rect().topRight())
        rect.moveRight(rect.right() - right - 1)
        self._searchToolWidget.setGeometry(rect)
        cr = self.contentsRect()
        self._lineNumberArea.setGeometry(QRect(cr.left(), cr.top(), self.lineNumberAreaWidth(), cr.height()))

    def focusInEvent(self, event: QFocusEvent):
        super().focusInEvent(event)
        self._selectCursor = QTextCursor()
        self._highlighter.setSelectCursor(self._selectCursor)
        self.search(str(self._findData), self._findFlags, self._isRegExp)

    def focusOutEvent(self, *args, **kwargs):
        super().focusInEvent(*args, **kwargs)
        self._selectCursor = QTextCursor()
        self._highlighter.setSelectCursor(self._selectCursor)
        self.search(str(self._findData), self._findFlags, self._isRegExp)

    def findItemClicked(self, item: FindItemInfo):
        block = self.document().findBlockByLineNumber(item.lineNumber)
        cursor = QTextCursor(block)
        cursor.setPosition(block.position() + item.findBegin)
        cursor.setPosition(block.position() + item.findEnd, QTextCursor.MoveMode.KeepAnchor)
        self._highlighter.setNextCursor(cursor)
        self._highlighter.rehighlight()
        cursor.clearSelection()
        self.setTextCursor(cursor)
        self.ensureCursorVisible()
        vScrollBar = self.verticalScrollBar()
        cursorY = self.cursorRect(cursor).top()
        scrollValue = vScrollBar.value() + cursorY - self.viewport().rect().top()
        vScrollBar.setValue(scrollValue)
        self.ensureCursorVisible()
        vScrollBar = self.verticalScrollBar()
        cursorY = self.cursorRect(cursor).top()
        scrollValue = vScrollBar.value() + cursorY - self.viewport().rect().top()
        vScrollBar.setValue(scrollValue)
    def textChangedSlot(self):
        pass