import QtQuick 2.3;

import ParserTools 1.0;

Reconstructor {
    id: reconstructor;

    property variant viewer: ({});

    onFinished: {
        viewer.drawModel(model);
        toggleDocks();
    }
}
