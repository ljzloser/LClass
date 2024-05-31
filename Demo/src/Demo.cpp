#include "Demo.h"
#include <QTimer>
#include <LWidget>
#include <QLinearGradient>
#include <QMessageBox>
Demo::Demo(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setWindowIcon(QIcon(":/Demo/res/start.png"));
	connect(ui.listWidget, &QListWidget::itemClicked, this, &Demo::doWork);
	QStringList list = {
		"LRingProgressBar(环形进度条)",
		"LWaveProgressBar(海浪进度条)",
		"LFileLineEdit(文件选择文本框)",
#ifdef WIN32
		"LWidget(自定义窗口)",
#endif
		"LPixmapButton(图片按钮)"
	};
	ui.listWidget->addItems(list);
}
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
	else if (text.contains("LRingProgressBar"))
	{
		LRingProgressBar* progress = new LRingProgressBar(this);
		progress->setMinimum(0);
		progress->setMaximum(100);
		progress->setFormat("%v/%m\n%p%");

		LRingProgressBar::ColorInfo info;
		QConicalGradient gradient = QConicalGradient(0, 0, 0);
		gradient.setColorAt(0.0, QColor(255, 0, 0));
		gradient.setColorAt(0.5, QColor(255, 255, 0));
		gradient.setColorAt(1.0, QColor(0, 255, 0));
		info.bodyBrush = QBrush(gradient);
		progress->setColorInfo(info);

		QTimer* timer = new QTimer(progress);
		connect(timer, &QTimer::timeout, [progress]() {
			progress->addValue();
			});
		timer->start(100);
		widget = progress;
	}
	else if (text.contains("LWaveProgressBar"))
	{
		LWaveProgressBar* progress = new LWaveProgressBar(this);
		progress->setMinimum(0);
		progress->setMaximum(100);
		QTimer* timer = new QTimer(progress);
		connect(timer, &QTimer::timeout, [progress]() {
			progress->addValue();
			});
		timer->start(100);
		widget = progress;
	}
	else if (text.contains("LFileLineEdit"))
	{
		LFileLineEdit* edit = new LFileLineEdit(this);
		LFileLineEdit::Info info;
		info.mode = QFileDialog::ExistingFiles;
		info.filters = { "*.exe"," *.pdb" };
		edit->setInfo(info);
		connect(edit, &LFileLineEdit::fileSelected, [edit](const QStringList& list, const QString& filter) {
			QMessageBox::information(edit, "选择文件", edit->selectedFiles("\n") + "\n" + filter + "\n" + QString::number(edit->info().mode));
			});
		widget = edit;
	}
#ifdef WIN32
	else if (text.contains("LWidget"))
	{
		LTitleBar* titleBar = new LTitleBar();
		QWidget* mainWidget = new QWidget();
		LWidget* _widget = new LWidget(titleBar, mainWidget);
		_widget->show();
	}
#endif
	else if (text.contains("LPixmapButton"))
	{
		LPixmapButton* pixmapButton = new LPixmapButton();

		pixmapButton->setPixmap(QPixmap(":/Demo/res/start.png"));
		widget = pixmapButton;
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

Demo::~Demo()
{}
