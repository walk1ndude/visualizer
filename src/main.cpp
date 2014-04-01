#include <QtGui/QGuiApplication>

#include <QtQml/QQmlApplicationEngine>

#include "dicomreader.h"
#include "glviewer.h"

int main(int argc, char * argv[]) {
    QGuiApplication a(argc, argv);

    QQmlApplicationEngine engine(QUrl("qrc:/qml/MainWindow.qml"));
    QObject * appWindow = engine.rootObjects().at(0);

    DicomReader dicomReader;

    QSurfaceFormat format;
    format.setSamples(16);
    format.setVersion(2, 1);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);

    GLviewer glviewer;

    glviewer.setFormat(format);
    glviewer.resize(640, 640);

    QObject::connect(appWindow, SIGNAL(fileOpened(QString)), &dicomReader, SLOT(readFile(QString)));
    QObject::connect(appWindow, SIGNAL(sliceNumberChanged(int)), &dicomReader, SLOT(changeSliceNumber(int)));

    QObject::connect(&dicomReader, &DicomReader::slicesProcessed, &glviewer, &GLviewer::drawSlices);

    static_cast<QWindow*>(appWindow)->show();

    return a.exec();
}
