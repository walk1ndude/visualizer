#include "UserUI/AppWindow.h"
#include "UserUI/ModelViewer.h"
#include "UserUI/ConsoleLogger.h"

#include "Parser/DicomReader.h"
#include "Parser/StlReader.h"
#include "Parser/Reconstructor.h"

#include <QtNetwork/QHostAddress>

namespace UserUI {
    AppWindow::AppWindow(const QString & qmlSource, const QString & host, const qint16 & port, QObject * parent) :
        QObject(parent) {

        //qInstallMessageHandler(Quick::ConsoleLogger::customMessageHandler);

        registerMetaTypes();
        registerQmlTypes();

        _engine = new QQmlApplicationEngine(QUrl(qmlSource), this);

        _netUI = new NetUI(host, port, this);

        fetchConnections();
    }

    QQuickWindow * AppWindow::appWindow() const {
        return _appWindow;
    }

    void AppWindow::fetchConnections() {
        QObject * appWindow = _engine->rootObjects().at(0);

        _appWindow = qobject_cast<QQuickWindow *>(appWindow);

#ifdef Q_OS_OSX
        QSurfaceFormat surfaceFormat = _appWindow->format();
        surfaceFormat.setVersion(4, 1);
        surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
        surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);

        _appWindow->setFormat(surfaceFormat);
#endif

        QObject::connect(appWindow, SIGNAL(recieve(const QVariant &)), this, SLOT(recieve(const QVariant &)));
    }

    void AppWindow::registerQmlTypes() {
        qmlRegisterType<UserUI::ModelViewer>("RenderTools", 1, 0, "ModelViewer");
        qmlRegisterType<UserUI::ConsoleLogger>("RenderTools", 1, 0, "ConsoleLogger");

        qmlRegisterType<Viewport::ViewportArray>("RenderTools", 1, 0, "ViewportArray");
        qmlRegisterType<Viewport::Viewport>("RenderTools", 1, 0, "Viewport");

        qmlRegisterType<Parser::DicomReader>("ParserTools", 1, 0, "DicomReader");
        qmlRegisterType<Parser::StlReader>("ParserTools", 1, 0, "StlReader");
        qmlRegisterType<Parser::Reconstructor>("ParserTools", 1, 0, "Reconstructor");

        qmlRegisterType<Scene::ModelScene>("RenderTools", 1, 0, "ModelScene");
    }

    void AppWindow::registerMetaTypes() {
        qRegisterMetaType<Message::SettingsMessage>("Message::SettingsMessage");
        qRegisterMetaType<Message::SettingsMessage::PostCriteria>("Message::SettingsMessage::PostCriteria");
    }

    void AppWindow::recieve(const QVariant & message) {
        recieve(Message::SettingsMessage::toMessage(message));
    }

    void AppWindow::recieve(const Message::SettingsMessage & message) {
        if (message.isReliable() && message.reciever().startsWith("appWindow")) {
            QString action = message.data["action"].toString();

            if (action == "updatePoint") {
                QVariantMap paramsMap = message.data["params"].toMap();

                PointsInfo::Position3D position = paramsMap["position"].value<PointsInfo::Position3D>();

                QVariantList positionJSON = { position.x(), position.y(), position.z() };

                paramsMap["position"] = QVariant(positionJSON);

                emit post(QJsonObject::fromVariantMap(paramsMap));

            }
            else if (action == "updateDists") {
                emit post(QJsonObject::fromVariantMap(message.data["params"].toMap()));
            }

            _netUI->recieve(message);
        }
    }

    void AppWindow::show() {
        _appWindow->show();
    }
}
