#include <QtQuick/QQuickWindow>

#include "Gui/AppWindow.h"

#include "Quick/ModelViewer.h"

#include "Parser/DicomReader.h"
#include "Parser/StlReader.h"

namespace Gui {
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

        QSurfaceFormat surfaceFormat = _appWindow->format();
        surfaceFormat.setVersion(4, 1);
        surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
        surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
        surfaceFormat.setSwapInterval(10);

        _appWindow->setFormat(surfaceFormat);

        QObject::connect(appWindow, SIGNAL(filesOpened(QVariant)), this, SLOT(readFiles(QVariant)));

        QObject::connect(appWindow, SIGNAL(distsUpdated(const QVariant &)), this, SLOT(updateDists(const QVariant &)));

        for (QObject * modelItem : _appWindow->findChild<QQuickItem *>("modelRow")->children()) {
            Quick::ModelViewer * modelViewer = modelItem->findChild<Quick::ModelViewer *>("modelViewer");

            QObject::connect(modelViewer, &Quick::ModelViewer::pointUpdated, this, &AppWindow::updatePoint);
        }
    }

    void AppWindow::updatePoint(const QVariantMap & point) {
        // need to serialize our vector3d
        QVariantMap pointM(point);

        QVector3D vector = qvariant_cast<QVector3D>(point["position"]);
        QVariantList vectorV;

        vectorV.append(vector.x());
        vectorV.append(vector.y());
        vectorV.append(vector.z());

        pointM.insert("position", vectorV);

        pointUpdated(QJsonObject::fromVariantMap(pointM));
    }

    void AppWindow::updateDists(const QVariant & dists) {
        distsUpdated(QJsonObject::fromVariantMap(qvariant_cast<QVariantMap>(dists)));
    }

    void AppWindow::readFiles(QVariant fileNames) {
        QStringList fileNamesStr;

        for (const QVariant & item : fileNames.value<QList<QUrl> >()) {
            fileNamesStr.append(item.toUrl().toLocalFile());
        }

        emit filesOpened(fileNamesStr);
    }

    void AppWindow::registerQmlTypes() {
        qmlRegisterType<Quick::ModelViewer>("RenderTools", 1, 0, "ModelViewer");
        qmlRegisterType<Viewport::ViewportArray>("RenderTools", 1, 0, "ViewportArray");
        qmlRegisterType<Viewport::Viewport>("RenderTools", 1, 0, "Viewport");

        qmlRegisterType<Parser::DicomReader>("ParserTools", 1, 0, "DicomReader");
        qmlRegisterType<Parser::StlReader>("ParserTools", 1, 0, "StlReader");
    }

    void AppWindow::show() {
        _appWindow->show();
    }
}
