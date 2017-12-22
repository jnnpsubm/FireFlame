#include "DSExplorer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DSExplorer w;
    w.show();
    return a.exec();
}
