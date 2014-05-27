#include <QtGui/QGuiApplication>

#include "appwindow.h"
#include "dicomreader.h"
#include "reconstructor.h"

int main(int argc, char * argv[]) {
    QGuiApplication a(argc, argv);

    AppWindow appWindow("qrc:/qml/MainWindow.qml");

    DicomReader dicomReader;
    Reconstructor reconstructor;

    QObject::connect(&appWindow, &AppWindow::fileOpened, &dicomReader, &DicomReader::readFile);
    QObject::connect(&appWindow, &AppWindow::sliceNumberChanged, &dicomReader, &DicomReader::changeSliceNumber);
    QObject::connect(&appWindow, &AppWindow::minHUChanged, &dicomReader, &DicomReader::updateMinHU);
    QObject::connect(&appWindow, &AppWindow::maxHUChanged, &dicomReader, &DicomReader::updateMaxHU);

    QObject::connect(&appWindow, &AppWindow::filesOpened, &reconstructor, &Reconstructor::readFiles);

    QObject::connect(&dicomReader, &DicomReader::slicesProcessed, &appWindow, &AppWindow::slicesProcessed);
    QObject::connect(&reconstructor, &Reconstructor::slicesProcessed, &appWindow, &AppWindow::slicesProcessed);

    appWindow.show();

    return a.exec();
}
