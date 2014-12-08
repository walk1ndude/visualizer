#include <QtGui/QGuiApplication>

#include <QtCore/QCommandLineParser>

#include "UserUI/AppWindow.h"

int main(int argc, char * argv[]) {
    QGuiApplication a(argc, argv);
    QGuiApplication::setApplicationVersion("visualizer");
    QGuiApplication::setApplicationVersion("0.99");

    QCommandLineParser parser;
    parser.setApplicationDescription(QGuiApplication::translate("main", "visualizer of dcm / stl data, slice reconstructor, and some more ;)"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption hostOption(QCommandLineOption(QStringList() << "H" << "host",
                                                     QGuiApplication::translate("main",
                                                                                (std::string("Specify host for network (default is ") +
                                                                                std::string(DEFAULT_HOST) + std::string(").")).c_str()),
                                                     QGuiApplication::tr("ip", "host"), DEFAULT_HOST));

    QCommandLineOption portOption(QCommandLineOption(QStringList() << "p" << "port",
                                                     QGuiApplication::translate("main", (std::string("Specify port for network (default is ")
                                                                                + std::to_string(DEFAULT_PORT) + std::string(").")).c_str()),
                                                     QGuiApplication::tr("number", "port"), QString::number(DEFAULT_PORT)));
    parser.addOption(hostOption);
    parser.addOption(portOption);

    parser.process(a);

    UserUI::AppWindow appWindow("qrc:/qml/MainWindow.qml",
                                QString::fromStdString(parser.value(hostOption).toStdString()),
                                parser.value(portOption).toInt()
                                );

    QObject::connect(&appWindow, &UserUI::AppWindow::pointUpdated, [](const QJsonObject & point) { qDebug() << "JSON: " << point;});
    QObject::connect(&appWindow, &UserUI::AppWindow::distsUpdated, [](const QJsonObject & dists) { qDebug() << "JSON: " << dists;});

    appWindow.show();

    return a.exec();
}
