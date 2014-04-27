import QtQuick 2.2

import TomographyTools 1.0

Item {
    id: sliceItem

    Rectangle {

        Hud {
            id: sliceHud
            objectName: "sliceHud"

            width: sliceItem.width
            height: sliceItem.height * 0.25

            visible: false
        }

        Rectangle {
            id: horizontalSeparator

            color: "#FF888888"
            anchors.top: sliceHud.top
            x: sliceItem.width / 2 - 5
            width: 10
            height: sliceItem.height

            visible: false

            z: -1
        }

        Rectangle {
            id: verticalSeparator

            color: "#FF888888"
            y: sliceItem.height / 2 - 22
            height: 10
            width: sliceItem.width

            visible: false

            z: -1
        }

        SliceViewer {
            id: sliceViewer
            objectName: "sliceViewer"

            property bool takeShots: false

            Behavior on takeShots {
                NumberAnimation {
                    duration: 1000
                }
            }

            anchors.right: parent.right

            width: 640
            height: 640
            z: -2
        }
    }

    function show() {
        sliceHud.visible = !sliceHud.visible;
        horizontalSeparator.visible = !horizontalSeparator.visible;
        verticalSeparator.visible = !verticalSeparator.visible;
    }

}
