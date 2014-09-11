#include "Quick/TextureNode.h"

TextureNode::TextureNode(QQuickWindow * window) :
    _size(0, 0),
    _fboTexId(GLuint(0)),
    _texture(nullptr),
    _window(window) {

    // Our texture node must have a texture, so use the default 0 texture.
    _texture = _window->createTextureFromId(0, QSize(1, 1));
    setTexture(_texture);
    setFiltering(QSGTexture::Linear);
}

TextureNode::~TextureNode() {
    delete _texture;
}

// This function gets called on the FBO rendering thread and will store the
// texture id and size and schedule an update on the window.
void TextureNode::newTexture(const GLuint & fboTexId, const QSize & size) {
    _textureMutex.lock();
    _fboTexId = fboTexId;
    _size = size;
    _textureMutex.unlock();

    // We cannot call QQuickWindow::update directly here, as this is only allowed
    // from the rendering thread or GUI thread.
    emit pendingNewTexture();
}

void TextureNode::prepareNode() {
    _textureMutex.lock();
    int newId = _fboTexId;
    QSize size = _size;
    _fboTexId = 0;
    _textureMutex.unlock();

    if (newId) {
        delete _texture;
        _texture = _window->createTextureFromId(newId, size);

        setTexture(_texture);
    }
}

