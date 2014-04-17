#include <QtQuick/QQuickWindow>

#include "appwindow.h"
#include "sliceviewer.h"

AppWindow::AppWindow(const QString & qmlSource, QObject * parent) :
    QObject(parent) {

    registerQmlTypes();

    _engine = new QQmlApplicationEngine(QUrl(qmlSource));

    fetchConnections();
}

AppWindow::~AppWindow() {
    delete _engine;
}

void AppWindow::fetchConnections() {
    QObject * appWindow = _engine->rootObjects().at(0);

    _appWindow = qobject_cast<QQuickWindow *>(appWindow);

    foreach (QObject * sliceItem, _appWindow->findChild<QQuickItem *>("sliceRow")->children()) {
        SliceViewer * sliceViewer = sliceItem->findChild<SliceViewer *>("sliceViewer");
        QObject * hud = sliceItem->findChild<QObject *>("sliceHud");

        QObject::connect(appWindow, SIGNAL(fileOpened(QString)), this, SIGNAL(fileOpened(QString)));
        QObject::connect(appWindow, SIGNAL(sliceNumberChanged(int)), this, SIGNAL(sliceNumberChanged(int)));

        QObject::connect(appWindow, SIGNAL(readyToQuit()), sliceViewer, SLOT(destroyContext()));

        //QObject::connect(appWindow, &QQuickWindow::destroyed, sliceViewer, &SliceViewer::destroyContext);

        QObject::connect(sliceViewer, &SliceViewer::destroyed, [=]() { qApp->quit(); });

        QObject::connect(hud, SIGNAL(maxValueChanged(int)), this, SIGNAL(maxValueChanged(int)));
        QObject::connect(hud, SIGNAL(minValueChanged(int)), this, SIGNAL(minValueChanged(int)));

        QObject::connect(hud, SIGNAL(angleChanged(qreal,qreal,qreal)), sliceViewer, SLOT(updateAngle(qreal,qreal,qreal)));
        QObject::connect(hud, SIGNAL(ambientIntensityChanged(qreal)), sliceViewer, SLOT(updateAmbientIntensity(qreal)));
        QObject::connect(hud, SIGNAL(zoomZChanged(qreal)), sliceViewer, SLOT(updateZoomZ(qreal)));

        QObject::connect(this, &AppWindow::slicesProcessed, sliceViewer, &SliceViewer::drawSlices);

        QObject::connect(sliceViewer, SIGNAL(initialized()), sliceItem, SLOT(show()));

        QObject::connect(_appWindow, &QQuickWindow::heightChanged, [=](const int & height) {
            sliceViewer->setHeight(height);
            sliceViewer->update();
        });

        QObject::connect(_appWindow, &QQuickWindow::widthChanged, [=](const int & width) {
            sliceViewer->setWidth(width);
            sliceViewer->update();
        });

        QObject::connect(_appWindow, &QQuickWindow::visibilityChanged, [=](const QWindow::Visibility & visibility) {
            if (visibility != (QWindow::Minimized || QWindow::Hidden)) {
                sliceViewer->update();
            }
        });
    }
}

void AppWindow::registerQmlTypes() {
    qmlRegisterType<SliceViewer>("TomographyTools", 1, 0, "SliceViewer");
}

void AppWindow::show() {
    _appWindow->show();
}
