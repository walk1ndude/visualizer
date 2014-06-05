#include <QtQuick/QQuickWindow>

#include "appwindow.h"
#include "modelviewer.h"

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

    QObject::connect(appWindow, SIGNAL(fileOpenedDcm(const QUrl &)), this, SIGNAL(fileOpenedDcm(const QUrl &)));
    QObject::connect(appWindow, SIGNAL(fileOpenedStl(const QUrl &)), this, SIGNAL(fileOpenedStl(const QUrl &)));

    QObject::connect(appWindow, SIGNAL(filesOpened(QVariant)), this, SLOT(readFiles(QVariant)));

    QObject::connect(appWindow, SIGNAL(sliceNumberChanged(const int &)), this, SIGNAL(sliceNumberChanged(const int &)));

    foreach (QObject * modelItem, _appWindow->findChild<QQuickItem *>("modelRow")->children()) {
        ModelViewer * modelViewer = modelItem->findChild<ModelViewer *>("modelViewer");

        QObject::connect(this, &AppWindow::slicesProcessed, modelViewer, &ModelViewer::drawSlices);

        QObject::connect(modelViewer, &ModelViewer::minHUChanged, this, &AppWindow::minHUChanged);
        QObject::connect(modelViewer, &ModelViewer::maxHUChanged, this, &AppWindow::maxHUChanged);

        QObject::connect(modelViewer, SIGNAL(initialized()), modelItem, SLOT(show()));

        QObject::connect(_appWindow, &QQuickWindow::heightChanged, [=](const int & height) {
            modelViewer->setHeight(height);
            modelViewer->update();
        });

        QObject::connect(_appWindow, &QQuickWindow::widthChanged, [=](const int & width) {
            modelViewer->setWidth(width);
            modelViewer->update();
        });

        QObject::connect(_appWindow, &QQuickWindow::visibilityChanged, [=](const QWindow::Visibility & visibility) {
            if (visibility != (QWindow::Minimized || QWindow::Hidden)) {
                modelViewer->update();
            }
        });
    }
}

void AppWindow::readFiles(QVariant fileNames) {
    QStringList fileNamesStr;

    foreach (QVariant item, fileNames.value<QList<QUrl> >()) {
        fileNamesStr.append(item.toUrl().toLocalFile());
    }

    emit filesOpened(fileNamesStr);
}

void AppWindow::registerQmlTypes() {
    qmlRegisterType<ModelViewer>("RenderTools", 1, 0, "ModelViewer");
}

void AppWindow::show() {
    _appWindow->show();
}
