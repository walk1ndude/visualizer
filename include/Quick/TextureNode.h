#ifndef TEXTURENODE_H
#define TEXTURENODE_H

#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>

class TextureNode : public QObject, public QSGSimpleTextureNode {
    Q_OBJECT
public:
    TextureNode(QQuickWindow * window) :
        _size(0, 0),
        _texture(nullptr),
        _window(window) {

        // Our texture node must have a texture, so use the default 0 texture.
        _texture = _window->createTextureFromId(0, QSize(1, 1));
        setTexture(_texture);
        setFiltering(QSGTexture::Linear);
    }

    ~TextureNode() {
            delete _texture;
    }

signals:
    void pendingNewTexture();

public slots:
    // This function gets called on the FBO rendering thread and will store the
    // texture id and size and schedule an update on the window.
    void newTexture(const GLuint & fboTexId, const QSize & size) {
        _textureMutex.lock();
        _fboTexId = fboTexId;
        _size = size;
        _textureMutex.unlock();

        // We cannot call QQuickWindow::update directly here, as this is only allowed
        // from the rendering thread or GUI thread.
        emit pendingNewTexture();
    }

    void prepareNode() {
        QMutexLocker locker (&_textureMutex);

        delete _texture;
        _texture = _window->createTextureFromId(_fboTexId, _size);
        setTexture(_texture);
    }

private:
    QSize _size;

    GLuint _fboTexId;

    QMutex _textureMutex;

    QSGTexture * _texture;
    QQuickWindow * _window;
};


#endif // TEXTURENODE_H
