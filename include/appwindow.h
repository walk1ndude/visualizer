#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QtQml/QQmlApplicationEngine>

#include <QtQuick/QQuickWindow>

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
    void fileOpened(const QString & fileName);
    void sliceNumberChanged(const int & number);

    void minHUChanged(const int & minHU);
    void maxHUChanged(const int & maxHU);

    void slicesProcessed(QSharedPointer<uchar> mergedData,
                         const std::vector<float> & scaling, const std::vector<size_t> & size,
                         const int & alignment, const size_t & rowLength,
                         const std::vector<int> & huRange = std::vector<int>());
};

#endif // APPWINDOW_H
