#include <QtGui/QGuiApplication>

#include "appwindow.h"
#include "dicomreader.h"

int main(int argc, char * argv[]) {
    QGuiApplication a(argc, argv);

    AppWindow appWindow("qrc:/qml/MainWindow.qml");

    DicomReader dicomReader;

    QObject::connect(&appWindow, &AppWindow::fileOpened, &dicomReader, &DicomReader::readFile);
    QObject::connect(&appWindow, &AppWindow::sliceNumberChanged, &dicomReader, &DicomReader::changeSliceNumber);
    QObject::connect(&appWindow, &AppWindow::minValueChanged, &dicomReader, &DicomReader::updateMinValue);
    QObject::connect(&appWindow, &AppWindow::maxValueChanged, &dicomReader, &DicomReader::updateMaxValue);

    QObject::connect(&dicomReader, &DicomReader::slicesProcessed, &appWindow, &AppWindow::slicesProcessed);

    appWindow.show();

    return a.exec();
}
