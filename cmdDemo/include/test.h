#include <QApplication>
#include <LInstanceMap.h>
#include <QThread>
#include <QTableWidget>
#include <QRandomGenerator>
using namespace ljz;
class Task : public QThread
{
private:
    /* data */
public:
    Task(/* args */);
    ~Task();
    void run();
};

Task::Task(/* args */)
{
}

Task::~Task()
{
}

void Task::run()
{
    while (true)
    {
        QThread::msleep(10);
        LInstanceMap::instance().set(QString::number(QRandomGenerator::global()->bounded(0,20)),QRandomGenerator::global()->generate(),true);
        LInstanceMap::instance().remove(QString::number(QRandomGenerator::global()->bounded(0,2)));
    }
}
class Table: public QTableWidget
{
public:
    Table(QWidget *parent = nullptr) : QTableWidget(parent)
    {
        this->setColumnCount(5);
        this->setHorizontalHeaderLabels({"key", "value", "oldValue", "time", "threadId"});
    }
    ~Table()
    {
    }
    void addRow(const Content &content);
    void removeRow(const Content &content);
    void updateRow(const Content &content);
    void valueChanged(Content content);
};



void Table::addRow(const Content &content)
{
    this->insertRow(this->rowCount());
    this->setItem(this->rowCount() - 1, 0, new QTableWidgetItem(content._key));
    this->setItem(this->rowCount() - 1, 1, new QTableWidgetItem(content._value.toString()));
    this->setItem(this->rowCount() - 1, 2, new QTableWidgetItem(content._oldValue.toString()));
    this->setItem(this->rowCount() - 1, 3, new QTableWidgetItem(content._time.toString("yyyy-MM-dd hh:mm:ss zz")));
    this->setItem(this->rowCount() - 1, 4, new QTableWidgetItem(QString::number(content._threadId)));
}

void Table::removeRow(const Content &content)
{
    for (int i = 0; i < this->rowCount(); ++i)
    {
        if (this->item(i, 0)->text() == content._key)
        {
            // 删除行
            QTableWidget::removeRow(i); 
            break;
        }
    }
}
void Table::updateRow(const Content &content)
{
    for (int i = 0; i < this->rowCount(); ++i)
    {
        if (this->item(i, 0)->text() == content._key)
        {
            this->setItem(i, 1, new QTableWidgetItem(content._value.toString()));
            this->setItem(i, 2, new QTableWidgetItem(content._oldValue.toString()));
            this->setItem(i, 3, new QTableWidgetItem(content._time.toString("yyyy-MM-dd hh:mm:ss zz")));
            this->setItem(i, 4, new QTableWidgetItem(QString::number(content._threadId)));
            break;
        }
    }
}
void Table::valueChanged(Content content){

    switch (content._type)
    {
    case Content::Type::Insert:
        this->addRow(content);
        break;
    case Content::Type::Update:
        this->updateRow(content);
        break;
    case Content::Type::Delete:
        this->removeRow(content);
        break;
    default:
        break;
    }
}