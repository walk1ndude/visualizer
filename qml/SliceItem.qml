import QtQuick 2.1

import TomographyTools 1.0

Item {

    Hud {
        id: sliceHud
        objectName: "sliceHud"
    }

    SliceViewer {
        id: sliceViewer
        objectName: "sliceViewer"

        width: 640
        height: 640
    }
}
