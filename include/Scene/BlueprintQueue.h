#ifndef BLUEPRINTQUEUE_H
#define BLUEPRINTQUEUE_H

#include "Info/SceneInfo.h"

#include <QtCore/QQueue>

namespace Scene {
    using QueueSize = int;

    class BlueprintQueue {
    public:
        BlueprintQueue(const QueueSize & size = 10);

        virtual void enqueue(const SceneInfo::Blueprint & blueprint) final;
        virtual SceneInfo::Blueprint dequeue() final;

        virtual bool isEmpty() const final;

        QueueSize size() const;

    private:
        QueueSize _size;

        QQueue<SceneInfo::Blueprint> _queue;

    public slots:
        virtual void setSize(const QueueSize & size);
    };
}

#endif // BLUEPRINTQUEUE_H
