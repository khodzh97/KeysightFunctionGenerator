#include "mainwindow.h"
#include "keysight33600a.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Keysight33600A gena;
    MainWindow w;

    QObject::connect(&gena, SIGNAL(passSocketState(QString, bool)), &w, SLOT(receiveSocketState(QString, bool)));

    QObject::connect(&w, SIGNAL(passSetParameters(QString, QString, QString, int, float)),
                     &gena, SLOT(setParameters(QString, QString, QString, int, float)));

    QObject::connect(&gena, SIGNAL(passParameters(QMap <QString, QString>)), &w, SLOT(receiveParameters(QMap <QString, QString>)));

    QObject::connect(&w, SIGNAL(passParcelToSend(QString)), &gena, SLOT(send(QString)));

    QObject::connect(&w, SIGNAL(setOutput(bool)), &gena, SLOT(manageOutput(bool)));

    w.show();
    gena.trySetConnection(w.returnIP(), w.returnPort());

    return a.exec();
}
