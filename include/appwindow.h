#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QtQml/QQmlApplicationEngine>

#include <QtQuick/QQuickWindow>

#include "sliceinfo.h"

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

    void slicesProcessed(SliceInfo::SliceSettings sliceSettings);

private slots:
     void readFiles(QVariant fileNames);
};

#endif // APPWINDOW_H
