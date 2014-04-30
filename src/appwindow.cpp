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

        QObject::connect(appWindow, SIGNAL(fileOpened(const QString &)), this, SIGNAL(fileOpened(const QString &)));
        QObject::connect(appWindow, SIGNAL(sliceNumberChanged(const int &)), this, SIGNAL(sliceNumberChanged(const int &)));

        QObject::connect(this, &AppWindow::slicesProcessed, sliceViewer, &SliceViewer::drawSlices);

        QObject::connect(sliceViewer, &SliceViewer::minHUChanged, this, &AppWindow::minHUChanged);
        QObject::connect(sliceViewer, &SliceViewer::maxHUChanged, this, &AppWindow::maxHUChanged);

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
