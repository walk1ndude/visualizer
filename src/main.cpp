#include <QtGui/QGuiApplication>

#include <QtQml/QQmlApplicationEngine>

#include "dicomreader.h"
#include "sliceviewer.h"

#include "openglitem.h"

int main(int argc, char * argv[]) {
    QGuiApplication a(argc, argv);

    qmlRegisterType<SliceViewer>("TomographyTools", 1, 0, "SliceViewer");

    QQmlApplicationEngine engine(QUrl("qrc:/qml/MainWindow.qml"));
    QObject * appWindow = engine.rootObjects().at(0);

    QQuickWindow * window = qobject_cast<QQuickWindow *>(appWindow);

    SliceViewer * sliceViewer = window->findChild<SliceViewer *>("sliceViewer");

    DicomReader dicomReader;

    QObject::connect(appWindow, SIGNAL(fileOpened(QString)), &dicomReader, SLOT(readFile(QString)));
    QObject::connect(appWindow, SIGNAL(sliceNumberChanged(int)), &dicomReader, SLOT(changeSliceNumber(int)));

    QObject::connect(&dicomReader, &DicomReader::slicesProcessed, sliceViewer, &SliceViewer::drawSlices);

    window->show();

    return a.exec();
}
