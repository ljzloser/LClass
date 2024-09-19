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
        QThread::msleep(100);
        LInstanceMap::instance().set(QString::number(QRandomGenerator::global()->bounded(0,1)),QRandomGenerator::global()->generate(),true);
        LInstanceMap::instance().remove(QString::number(QRandomGenerator::global()->bounded(0,1)));
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
    QDateTime time;
    QMutex mutex;
    void addRow(const Content &content);
    void removeRow(const Content &content);
    void updateRow(const Content &content);
public slots:
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
    QMutexLocker locker(&mutex);
    if(content._time.toMSecsSinceEpoch()<=time.toMSecsSinceEpoch())
        qDebug() << "error";
	time = content._time;
  //  qDebug() << lastOp;
    switch (content._type)
    {
    case Content::Type::Insert:
    {
        //if (lastOp != "removeRow")
        //    qDebug() << "lastOp11111111111111111111111111111111111111111111111111";
       // lastOp = "addRow";
        this->addRow(content);
        break;
    }
    case Content::Type::Update:
       // lastOp = "updateRow";
        this->updateRow(content);
        break;
    case Content::Type::Delete:
       // lastOp = "removeRow";
        this->removeRow(content);
        break;
    default:
        break;
    }
}