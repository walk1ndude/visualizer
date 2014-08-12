#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QtQml/QQmlApplicationEngine>

#include <QtQuick/QQuickWindow>

#include <QtCore/QJsonObject>

#include "Info/SliceInfo.h"
#include "Info/ModelInfo.h"

#include "Model/StlModel.h"

namespace Gui {
    class AppWindow : public QObject {
        Q_OBJECT
    public:
        explicit AppWindow(const QString & qmlSource, QObject * parent = 0);
        ~AppWindow();

        void show();

    private:
        QQmlApplicationEngine * _engine;

        QQuickWindow * _appWindow;

        void registerQmlTypes();

        void fetchConnections();

    signals:
        void fileOpenedDcm(const QUrl & fileName);
        void fileOpenedStl(const QUrl & fileName);

        void filesOpened(const QStringList & fileNames);

        void sliceNumberChanged(const int & number);

        void minHUChanged(const int & minHU);
        void maxHUChanged(const int & maxHU);

        void slicesProcessed(SliceInfo::Slices sliceSettings);
        void modelRead(ModelInfo::BuffersVN buffers);

        void pointUpdated(const QJsonObject & json);
        void distsUpdated(const QJsonObject & json);

    private slots:
         void readFiles(QVariant fileNames);

         void updatePoint(const QVariantMap & point);
         void updateDists(const QVariant & dists);
    };
}

#endif // APPWINDOW_H
