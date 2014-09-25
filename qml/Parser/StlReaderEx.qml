import QtQuick 2.3;

import ParserTools 1.0;

StlReader {
    id: stlReader;

    property variant viewer: ({});

    onFinished: {
        if (viewer) {
            viewer.drawModel(model);
            toggleDocks();
            destroy();
        }
    }
}
