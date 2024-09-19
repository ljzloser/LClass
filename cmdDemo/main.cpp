#include <QApplication>
#include <QThread>
#include <QTableWidget>
#include "test.h"
using namespace ljz;
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Table table;
    for(int i = 0; i < 100 ; i ++)
    {
        Task * task1 = new Task();
        task1->start();
    }

    QObject::connect(&LInstanceMap::instance(), &LInstanceMap::valueChanged, &table, &Table::valueChanged,Qt::QueuedConnection);
    table.show();
    return a.exec();
}
