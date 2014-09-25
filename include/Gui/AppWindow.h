#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QtQml/QQmlApplicationEngine>

#include <QtQuick/QQuickWindow>

#include <QtCore/QJsonObject>

namespace Gui {
    class AppWindow : public QObject {
        Q_OBJECT
    public:
        explicit AppWindow(const QString & qmlSource, QObject * parent = 0);
        ~AppWindow();

        void show();

        QQuickWindow * appWindow() const;

    private:
        QQmlApplicationEngine * _engine;

        QQuickWindow * _appWindow;

        void registerQmlTypes();

        void fetchConnections();

    signals:
        void pointUpdated(const QJsonObject & json);
        void distsUpdated(const QJsonObject & json);

    private slots:
         void updatePoint(const QVariant & point);
         void updateDists(const QVariant & dists);
    };
}

#endif // APPWINDOW_H
