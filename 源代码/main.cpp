#include "ui.h"

#include <QApplication>
#include <string>
using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UI w;
    w.show();
    return a.exec();
}
