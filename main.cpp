#include "subnetquiz.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SubnetQuiz w;
    w.show();
    return a.exec();
}
