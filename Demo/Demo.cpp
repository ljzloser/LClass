#include "Demo.h"
#include <QPushButton>

void Demo::doWork(QListWidgetItem* item)
{
	const auto text = item->text();
	QWidget* widget = nullptr;
	if (text.contains("LMultiComboBox"))
	{
		LMultiComboBox* combo = new LMultiComboBox(this);
		combo->addItems({ "APPLE", "apple", "3" });
		widget = combo;
	}
	else if (text.contains("LCompleteComboBox"))
	{
		QComboBox* combo = new LCompleteComboBox(this);
		combo->setEditable(true);
		combo->addItems({ "APPLE", "apple", "3" });
		widget = combo;
	}
	else if (text.contains("LSwitchButton"))
	{
		LSwitchButton* button = new LSwitchButton(this);
		//LSwitchButton::StateInfo info;
		//info.text = "开始";
		//info.textColor = QColor(Qt::white);
		//info.backgroundColor = QColor(Qt::black);
		//info.buttonPixmap = QPixmap(":/Demo/res/start.png");
		//button->setStateInfo(LSwitchButton::State::On, info);
		//LSwitchButton::StateInfo stopInfo;
		//stopInfo.text = "停止";
		//stopInfo.textColor = QColor(Qt::black);
		//stopInfo.backgroundColor = QColor(Qt::white);
		//stopInfo.buttonPixmap = QPixmap(":/Demo/res/stop.png");
		//button->setStateInfo(LSwitchButton::State::Off, stopInfo);

		//button->setFixedHeight(50);
		button->setDisabled(false);
		//button->setOpenAnimation(false);
		widget = button;
	}
	else if (text.contains("LInstrumentBoard"))
	{
		LInstrumentBoard* board = new LInstrumentBoard(0, 100);
		board->setValue(80);
		board->setName("仪表盘");
		board->setUnit("°C");
		widget = board;
	}
	else if (text.contains("LTextEdit"))
	{
		LTextEdit* edit = new LTextEdit(this);
		edit->setPlaceholderText("请输入内容");
		edit->setPlainText(R"({
    name:asdasd,
    type:person,
    email:13123581@qq.com,
    type:person,
})");
		edit->setLineNumberAreaVisible(true);
		//edit->setHighlightCurrentLine(true);
		widget = edit;
	}
	if (widget)
	{
		delete ui.widget;
		ui.widget = widget;
		LAnimationOpacityEffect* effect = new LAnimationOpacityEffect(ui.widget);
		ui.widget->setGraphicsEffect(effect);
		effect->setOpacity(0);
		ui.gridLayout->addWidget(widget, 0, 1);
		effect->inAnimationStart();
	}
}

Demo::Demo(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.listWidget, &QListWidget::itemClicked, this, &Demo::doWork);
}

Demo::~Demo()
{}