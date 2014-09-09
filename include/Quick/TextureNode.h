#ifndef TEXTURENODE_H
#define TEXTURENODE_H

#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>

class TextureNode : public QObject, public QSGSimpleTextureNode {
    Q_OBJECT
public:
    explicit TextureNode(QQuickWindow * window);
    ~TextureNode();

signals:
    void pendingNewTexture();

public slots:
    void newTexture(const GLuint & fboTexId, const QSize & size);
    void prepareNode();

private:
    QSize _size;

    GLuint _fboTexId;

    QMutex _textureMutex;

    QSGTexture * _texture;
    QQuickWindow * _window;
};

#endif // TEXTURENODE_H
