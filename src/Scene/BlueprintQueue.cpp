#include "Scene/BlueprintQueue.h"

namespace Scene {
    BlueprintQueue::BlueprintQueue(const QueueSize & size) :
        _size(size) {

    }

    QueueSize BlueprintQueue::size() const {
        return _size;
    }

    bool BlueprintQueue::isEmpty() const {
        return _queue.isEmpty();
    }

    void BlueprintQueue::setSize(const QueueSize & size) {
        _size = size;
    }

    void BlueprintQueue::enqueue(const SceneInfo::Blueprint & blueprint) {
        while (_queue.size() >= _size) {
            _queue.dequeue();
        }

        _queue.enqueue(blueprint);
    }

    SceneInfo::Blueprint BlueprintQueue::dequeue() {
        return (!_queue.isEmpty()) ? _queue.dequeue() : SceneInfo::Blueprint();
    }
}
