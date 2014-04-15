import QtQuick 2.2

import TomographyTools 1.0

Item {

    id: sliceItem


    Rectangle {


        Hud {
            id: sliceHud
            objectName: "sliceHud"
        }

        SliceViewer {
            id: sliceViewer
            objectName: "sliceViewer"

            anchors.right: parent.right

            width: 640
            height: 640

            z: -1
        }
    }
}
