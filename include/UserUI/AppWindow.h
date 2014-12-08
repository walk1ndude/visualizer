#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QtQml/QQmlApplicationEngine>

#include <QtQuick/QQuickWindow>

#include <QtCore/QJsonObject>

#include "Message/SettingsMessage.h"

#include "UserUI/NetUI.h"

namespace UserUI {
    class AppWindow : public QObject {
        Q_OBJECT
    public:
        explicit AppWindow(const QString & qmlSource,
                           const QString & host = "127.0.0.1",
                           const qint16 & post = 5555,
                           QObject * parent = nullptr);
        void show();

        QQuickWindow * appWindow() const;

    private:
        QQmlApplicationEngine * _engine;

        QQuickWindow * _appWindow;

        NetUI * _netUI;

        void registerQmlTypes();
        void registerMetaTypes();

        void fetchConnections();

    signals:
        void pointUpdated(const QJsonObject & json);
        void distsUpdated(const QJsonObject & json);

    private slots:
         void updatePoint(const QVariant & point);
         void updateDists(const QVariant & dists);

         virtual void recieve(const Message::SettingsMessage & message);
         virtual void recieve(const QVariant & message);
    };
}

#endif // APPWINDOW_H
