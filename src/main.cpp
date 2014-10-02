#include <QtGui/QGuiApplication>

#include <QtCore/QDebug>

#include "Gui/AppWindow.h"

int main(int argc, char * argv[]) {
    QGuiApplication a(argc, argv);

    //qputenv("QML_FORCE_THREADED_RENDERER", "1");

    Gui::AppWindow appWindow("qrc:/qml/MainWindow.qml");

    QObject::connect(&appWindow, &Gui::AppWindow::pointUpdated, [](const QJsonObject & point) { qDebug() << "JSON: " << point;});
    QObject::connect(&appWindow, &Gui::AppWindow::distsUpdated, [](const QJsonObject & dists) { qDebug() << "JSON: " << dists;});

    appWindow.show();

    return a.exec();
}
