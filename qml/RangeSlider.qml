import QtQuick 2.3

import "../js/helperfuncs.js" as HelperFuncs

Item {
    id: sliderMain

    property vector2d valueRange: Qt.vector2d(-1.0, 1.0)
    property vector2d value: Qt.vector2d(-1.0, 1.0)

    width: 200
    height: 20

    Rectangle  {
        anchors {
            verticalCenter: parent.verticalCenter
            fill: parent
        }

        radius: 8

        gradient: Gradient  {
            GradientStop  {
                position: 0.0
                color: "gray"
            }

            GradientStop  {
                position: 1.0
                color: "white"
            }
        }

        z: -2
    }

    Rectangle  {
        id: handleMinValue

        x: (sliderMain.value.x - sliderMain.valueRange.x) * (sliderMain.width - handleMinValue.width) /
           (sliderMain.valueRange.y - sliderMain.valueRange.x)

        smooth: true

        anchors.top: parent.top

        width: 20
        height: sliderMain.height

        radius: parent.height / 2

        gradient: Gradient  {
            GradientStop  {
                position: 0.0
                color: "lightgray"
            }
            GradientStop  {
                position: 1.0
                color: "gray"
            }
        }

        MouseArea  {
            anchors.fill: parent

            drag {
                axis: Drag.XAxis
                target: parent
                minimumX: 0
                maximumX: sliderMain.width
            }

            onPositionChanged:  {
                value.x = (sliderMain.valueRange.y - sliderMain.valueRange.x) * handleMinValue.x /
                        (sliderMain.width - handleMinValue.width) + sliderMain.valueRange.x;
            }
        }
    }

    Rectangle  {
        id: handleMaxValue

        x: (sliderMain.value.y - sliderMain.valueRange.x) * (sliderMain.width - handleMaxValue.width) /
           (sliderMain.valueRange.y - sliderMain.valueRange.x)

        smooth: true

        anchors.top: parent.top

        width: 20
        height: sliderMain.height

        radius: parent.height / 2

        gradient: Gradient  {
            GradientStop  {
                position: 0.0
                color: "lightgray"
            }
            GradientStop  {
                position: 1.0
                color: "gray"
            }
        }

        MouseArea  {
            anchors.fill: parent

            drag {
                axis: Drag.XAxis
                target: parent
                minimumX: 0
                maximumX: sliderMain.width
            }

            onPositionChanged:  {
                value.y = (sliderMain.valueRange.y - sliderMain.valueRange.x) * handleMaxValue.x /
                        (sliderMain.width - handleMinValue.width) + sliderMain.valueRange.x;
            }
        }
    }

    Rectangle {
        x: handleMinValue.x + handleMinValue.width / 2

        anchors.top: parent.top
        height: parent.height

        width: handleMaxValue.x - handleMinValue.x

        gradient: Gradient  {
            GradientStop  {
                position: 0.0
                color: "cyan"
            }

            GradientStop  {
                position: 1.0
                color: "blue"
            }
        }

        z: -1
    }

    onValueChanged: clampValues()
    onValueRangeChanged: clampValues()

    function clampValues() {
        if (valueRange.x !== valueRange.y) {
            value.x = HelperFuncs.clamp(value.x, valueRange.x, value.y);
            value.y = HelperFuncs.clamp(value.y, value.x, valueRange.y);
        }
    }
}
