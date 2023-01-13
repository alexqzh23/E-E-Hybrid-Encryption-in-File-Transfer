#include "sendfileop.h"
#include <QtWidgets/QApplication>
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    sendfileop w;
    w.show();
    return a.exec();
}
