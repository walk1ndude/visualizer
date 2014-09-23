import QtQuick 2.3

import ParserTools 1.0

Reconstructor {
    id: reconstructor;

    property variant viewer: ({});

    onSlicesProcessed: {
        viewer.drawSlices(slices);
        toggleDocks();
    }
}
