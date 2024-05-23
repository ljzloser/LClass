#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Demo.h"
#include <LCore>
#include <LWidget>

class Demo : public QMainWindow
{
	Q_OBJECT

public:
	void doWork(QListWidgetItem* item);
	Demo(QWidget* parent = nullptr);
	~Demo();

private:
	Ui::DemoClass ui;
};
