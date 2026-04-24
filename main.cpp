// SUBNET QUIZ APP VERSION 1.0
// ALBERTO C. COSTA 2026

#include "subnetquiz.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SubnetQuiz w;
    w.show();
    return a.exec();
}
