import QtQuick 2.2

import TomographyTools 1.0

Item {
    id: sliceItem

    property bool takeShots: false

    Rectangle {

        Hud {
            id: sliceHud
            objectName: "sliceHud"

            width: sliceItem.width
            height: sliceItem.height * 0.25

            visible: false

            onAngleChanged: sliceViewer.rotation = sliceHud.angle;

            onZoomFactorChanged: sliceViewer.zoomFactor = sliceHud.zoomFactor;

            onSRangeChanged: sliceViewer.sRange = sliceHud.sRange;
            onTRangeChanged: sliceViewer.tRange = sliceHud.tRange;
            onPRangeChanged: sliceViewer.pRange = sliceHud.pRange;

            onMinValueChanged: sliceViewer.minValue = sliceHud.minValue;
            onMaxValueChanged: sliceViewer.maxValue = sliceHud.maxValue;
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

            anchors.right: parent.right

            width: 640
            height: 640
            z: -2
        }
    }

    Behavior on takeShots {
        //angleShotAnimation
    }

    function show() {
        sliceHud.visible = !sliceHud.visible;
        horizontalSeparator.visible = !horizontalSeparator.visible;
        verticalSeparator.visible = !verticalSeparator.visible;
    }

    NumberAnimation {
        id: angleShotAnimation

        target: sliceHud
        property: "angleShot"

        running: true
        duration: 1000

        from: 0.0
        to: 180.0
    }
}
