#include <QtGui/QGuiApplication>

#include "Gui/AppWindow.h"

#include "Parser/Reconstructor.h"

int main(int argc, char * argv[]) {
    QGuiApplication a(argc, argv);

    Gui::AppWindow appWindow("qrc:/qml/MainWindow.qml");

    Parser::Reconstructor reconstructor;

    QObject::connect(&appWindow, &Gui::AppWindow::filesOpened, &reconstructor, &Parser::Reconstructor::readFiles);

    QObject::connect(&reconstructor, &Parser::Reconstructor::slicesProcessed, &appWindow, &Gui::AppWindow::slicesProcessed);

    QObject::connect(&appWindow, &Gui::AppWindow::sliceNumberChanged, &reconstructor, &Parser::Reconstructor::changeSliceNumber);

    QObject::connect(&appWindow, &Gui::AppWindow::pointUpdated, [](const QJsonObject & point) { qDebug() << "JSON: " << point;});
    QObject::connect(&appWindow, &Gui::AppWindow::distsUpdated, [](const QJsonObject & dists) { qDebug() << "JSON: " << dists;});

    appWindow.show();

    return a.exec();
}
