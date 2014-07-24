import QtQuick 2.3

import "../js/helperfuncs.js" as HelperFuncs

Rectangle {
    id: sliderMain

    property real minumumValue: 0.0
    property real maximumValue: 1.0
    property real value: 0.0

    width: 140
    height: width

    color: "red"

    border {
        color: "black"
        width: 5
    }

    radius: width * 0.5

    Rectangle {
        width: parent.width * 0.7
        height: width
        color: "darkgrey"
        border.color: "black"
        border.width: 5
        radius: width * 0.5

        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }
    }

    Rectangle {
        id: innerCircle

        width: parent.width * 0.7
        height: width
        color: "darkgrey"
        border.color: "black"
        border.width: 5
        radius: width * 0.5

        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }
    }

    Rectangle {
        id: handle

        x: 70
        y: 2
        //x: parent.radius - width / 2
        //y: parent.border.width / 2

        width: (parent.width - innerCircle.width) / 4
        height: width * 2
        color: "darkgrey"
        border.color: "black"
        border.width: 2

        MouseArea  {
            anchors.fill: parent

            drag {
                axis: Drag.XAndYAxis
                target: parent
                minimumX: 0
                maximumX: sliderMain.width
                minimumY: 0
                maximumY: sliderMain.height
            }

            onPositionChanged:  {
                var originX = parent.radius - handle.x
                var originY = parent.radius - handle.y

                parent.x = Math.cos(1.57) * parent.x;
            }
        }
    }

    Text {
        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }

        text: parent.value
        font.pixelSize: 32
    }
}
