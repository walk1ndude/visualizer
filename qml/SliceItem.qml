import QtQuick 2.1

import TomographyTools 1.0

Item {

    id: sliceItem

    Hud {
        id: sliceHud
        objectName: "sliceHud"
    }

    SliceViewer {
        id: sliceViewer
        objectName: "sliceViewer"

        width: 640
        height: 640

        z: -1
    }
}
