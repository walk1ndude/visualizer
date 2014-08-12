#include <QtGui/QGuiApplication>

#include "Gui/AppWindow.h"

#include "Parser/DicomReader.h"
#include "Parser/Reconstructor.h"
#include "Parser/StlReader.h"

int main(int argc, char * argv[]) {
    QGuiApplication a(argc, argv);

    Gui::AppWindow appWindow("qrc:/qml/MainWindow.qml");

    Parser::DicomReader dicomReader;
    Parser::StlReader stlReader;

    Parser::Reconstructor reconstructor;

    QObject::connect(&appWindow, &Gui::AppWindow::fileOpenedDcm, &dicomReader, &Parser::DicomReader::readFile);
    QObject::connect(&appWindow, &Gui::AppWindow::fileOpenedStl, &stlReader, &Parser::StlReader::readFile);

    QObject::connect(&stlReader, &Parser::StlReader::readingErrorHappened, []() { qDebug() << "read error happend"; });

    QObject::connect(&appWindow, &Gui::AppWindow::sliceNumberChanged, &dicomReader, &Parser::DicomReader::changeSliceNumber);
    QObject::connect(&appWindow, &Gui::AppWindow::minHUChanged, &dicomReader, &Parser::DicomReader::updateMinHU);
    QObject::connect(&appWindow, &Gui::AppWindow::maxHUChanged, &dicomReader, &Parser::DicomReader::updateMaxHU);

    QObject::connect(&appWindow, &Gui::AppWindow::filesOpened, &reconstructor, &Parser::Reconstructor::readFiles);

    QObject::connect(&dicomReader, &Parser::DicomReader::slicesProcessed, &appWindow, &Gui::AppWindow::slicesProcessed);
    QObject::connect(&reconstructor, &Parser::Reconstructor::slicesProcessed, &appWindow, &Gui::AppWindow::slicesProcessed);
    QObject::connect(&stlReader, &Parser::StlReader::modelRead, &appWindow, &Gui::AppWindow::modelRead);

    QObject::connect(&appWindow, &Gui::AppWindow::sliceNumberChanged, &reconstructor, &Parser::Reconstructor::changeSliceNumber);

    QObject::connect(&appWindow, &Gui::AppWindow::pointUpdated, [](const QJsonObject & point) { qDebug() << "JSON: " << point;});
    QObject::connect(&appWindow, &Gui::AppWindow::distsUpdated, [](const QJsonObject & dists) { qDebug() << "JSON: " << dists;});

    appWindow.show();

    return a.exec();
}
