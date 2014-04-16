#include <QtGui/QGuiApplication>

#include <QtQml/QQmlApplicationEngine>

#include <QtQuick/QQuickWindow>

#include "dicomreader.h"
#include "sliceviewer.h"

int main(int argc, char * argv[]) {
    QGuiApplication a(argc, argv);

    qmlRegisterType<SliceViewer>("TomographyTools", 1, 0, "SliceViewer");

    QQmlApplicationEngine engine(QUrl("qrc:/qml/MainWindow.qml"));
    QObject * appWindow = engine.rootObjects().at(0);

    QQuickWindow * window = qobject_cast<QQuickWindow *>(appWindow);

    QObject * sliceItem = window->findChild<QObject *>("sliceItem");

    SliceViewer * sliceViewer = sliceItem->findChild<SliceViewer *>("sliceViewer");
    QObject * hud = sliceItem->findChild<QObject *>("sliceHud");

    DicomReader dicomReader;

    QObject::connect(appWindow, SIGNAL(fileOpened(QString)), &dicomReader, SLOT(readFile(QString)));
    QObject::connect(appWindow, SIGNAL(sliceNumberChanged(int)), &dicomReader, SLOT(changeSliceNumber(int)));

    QObject::connect(hud, SIGNAL(rTopChanged(qreal)), sliceViewer, SLOT(updateRTop(qreal)));
    QObject::connect(hud, SIGNAL(rBottomChanged(qreal)), sliceViewer, SLOT(updateRBottom(qreal)));

    QObject::connect(hud, SIGNAL(angleChanged(qreal,qreal,qreal)), sliceViewer, SLOT(updateAngle(qreal,qreal,qreal)));
    QObject::connect(hud, SIGNAL(ambientIntensityChanged(qreal)), sliceViewer, SLOT(updateAmbientIntensity(qreal)));
    QObject::connect(hud, SIGNAL(zoomZChanged(qreal)), sliceViewer, SLOT(updateZoomZ(qreal)));

    QObject::connect(&dicomReader, &DicomReader::slicesProcessed, sliceViewer, &SliceViewer::drawSlices);

    QObject::connect(sliceViewer, SIGNAL(initialized()), sliceItem, SLOT(show()));

    window->show();

    QObject::connect(window, &QQuickWindow::heightChanged, [=](const int & height) {
        sliceViewer->setHeight(height);
        sliceViewer->update();
    });

    QObject::connect(window, &QQuickWindow::widthChanged, [=](const int & width) {
        sliceViewer->setWidth(width);
        sliceViewer->update();
    });

    QObject::connect(window, &QQuickWindow::visibilityChanged, [=](const QWindow::Visibility & visibility) {
        if (visibility != (QWindow::Minimized || QWindow::Hidden)) {
            sliceViewer->update();
        }
    });

    return a.exec();
}
