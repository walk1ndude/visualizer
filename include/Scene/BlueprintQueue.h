#ifndef BLUEPRINTQUEUE_H
#define BLUEPRINTQUEUE_H

#include "Info/Info.h"

#include <QtCore/QQueue>

namespace Scene {
    using Blueprint = QVariantMap;

    using QueueSize = int;

    class BlueprintQueue {
    public:
        BlueprintQueue(const QueueSize & size = 10);

        virtual void enqueue(const Blueprint & blueprint) final;
        virtual Blueprint dequeue() final;

        virtual bool isEmpty() const final;

        QueueSize size() const;

    private:
        QueueSize _size;

        QQueue<Blueprint> _queue;

    public slots:
        virtual void setSize(const QueueSize & size);
    };
}

#endif // BLUEPRINTQUEUE_H
