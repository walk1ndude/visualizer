#include <QtGui/QGuiApplication>

#include "Gui/AppWindow.h"

int main(int argc, char * argv[]) {
    QGuiApplication a(argc, argv);

    Gui::AppWindow appWindow("qrc:/qml/MainWindow.qml");

    QObject::connect(&appWindow, &Gui::AppWindow::pointUpdated, [](const QJsonObject & point) { qDebug() << "JSON: " << point;});
    QObject::connect(&appWindow, &Gui::AppWindow::distsUpdated, [](const QJsonObject & dists) { qDebug() << "JSON: " << dists;});

    appWindow.show();

    return a.exec();
}
