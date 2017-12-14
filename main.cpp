#include <QCoreApplication>

#include "handler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Handler h;

    return a.exec();
}
