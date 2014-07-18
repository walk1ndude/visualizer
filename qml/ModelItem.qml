import QtQuick 2.3

import RenderTools 1.0

Item {
    id: modelItem

    property bool takeShot: false

    property vector2d xRange: Qt.vector2d(0.0, 1.0)
    property vector2d yRange: Qt.vector2d(0.0, 1.0)
    property vector2d zRange: Qt.vector2d(0.0, 1.0)

    property vector3d rotation: Qt.vector3d(0.0, 0.0, 0.0)
    property real zoomFactor: 2.0

    property int minHU: 0
    property int maxHU: 65535

    property string selectedPointName: ""
    property color selectedPointColor: Qt.rgba(0, 0, 0, 0)

    Rectangle {
        Rectangle {
            id: horizontalSeparator

            color: "#FF888888"
            x: modelItem.width / 2 - 5
            width: 10
            height: modelItem.height

            visible: false

            z: -1
        }

        Rectangle {
            id: verticalSeparator

            color: "#FF888888"
            y: modelItem.height / 2 - 5
            height: 10
            width: modelItem.width

            visible: false

            z: -1
        }

        ModelViewer {
            id: modelViewer
            objectName: "modelViewer"

            width: modelItem.width
            height: modelItem.height
            z: -2

            xRange: modelItem.xRange
            yRange: modelItem.yRange
            zRange: modelItem.zRange

            rotation: modelItem.rotation

            zoomFactor: modelItem.zoomFactor

            minHU: modelItem.minHU
            maxHU: modelItem.maxHU

            takeShot: modelItem.takeShot

            selectedPointColor: modelItem.selectedPointColor
            selectedPointName: modelItem.selectedPointName

            MouseArea {
                anchors.fill: parent
                onClicked: if (mouse.button == Qt.LeftButton) {
                               modelViewer.selectedPointPosition = Qt.point(mouseX, mouseY);
                           }
            }
        }
    }

    onTakeShotChanged: {
        if (modelItem.takeShot) {
            angleShotTimer.running = true;
        }
    }

    function show() {
        horizontalSeparator.visible = true;
        verticalSeparator.visible = true;
    }
/*
    Timer {
        id: angleShotTimer
        interval: 400

        repeat: true
        running: false

        onTriggered: {
            modelHud.angleShot += 1.0;

            if (modelHud.angleShot > 179.0) {
                angleShotTimer.running = false;
                modelHud.angleShot = 0.0;

                modelItemItem.takeShot = false;
            }
        }
    }*/
}
