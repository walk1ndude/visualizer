import QtQuick 2.2

import RenderTools 1.0

Item {
    id: modelItem

    property bool takeShot: false

    Rectangle {

        Hud {
            id: modelHud
            objectName: "sliceHud"

            width: modelItem.width
            height: modelItem.height * 0.35

            huRange: modelViewer.huRange

            visible: false
        }

        Rectangle {
            id: horizontalSeparator

            color: "#FF888888"
            anchors.top: modelHud.top
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

            sRange: modelHud.sRange
            tRange: modelHud.tRange
            pRange: modelHud.pRange

            rotation: modelHud.angle

            zoomFactor: modelHud.zoomFactor

            minHU: modelHud.minHU
            maxHU: modelHud.maxHU

            takeShot: modelItem.takeShot
        }
    }

    onTakeShotChanged: {
        if (sliceItem.takeShot) {
            angleShotTimer.running = true;
        }
    }

    function show() {
        modelHud.visible = !modelHud.visible;
        horizontalSeparator.visible = !horizontalSeparator.visible;
        verticalSeparator.visible = !verticalSeparator.visible;
    }

    Timer {
        id: angleShotTimer
        interval: 400

        repeat: true
        running: false

        onTriggered: {
            sliceHud.angleShot += 1.0;

            if (sliceHud.angleShot > 180.0) {
                angleShotTimer.running = false;
                sliceHud.angleShot = 0.0;

                sliceItem.takeShot = false;
            }
        }
    }
}
