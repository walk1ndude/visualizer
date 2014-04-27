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

    void minValueChanged(const int & minValue);
    void maxValueChanged(const int & maxValue);

    void slicesProcessed(QSharedPointer<uchar> mergedData, QSharedPointer<uchar> gradientData,
                         const std::vector<float> & scaling, const std::vector<size_t> & size,
                         const int & alignment, const size_t & rowLength,
                         const int & alignmentGradient, const size_t & rowLengthGradient);
};

#endif // APPWINDOW_H
