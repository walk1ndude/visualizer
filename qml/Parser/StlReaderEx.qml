import QtQuick 2.3

import ParserTools 1.0

StlReader {
    id: stlReader;

    property variant viewer: ({});

    onModelRead: {
        if (viewer) {
            viewer.modelRead(buffers);
            toggleDocks();
            destroy();
        }
    }
}
