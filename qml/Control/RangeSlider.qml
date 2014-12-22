import QtQuick 2.3;

import "qrc:/js/helpers.js" as Helpers;

Item {
    id: sliderMain;

    property vector2d valueRange: Qt.vector2d(-1.0, 1.0);
    property vector2d value: Qt.vector2d(-1.0, 1.0);

    width: 200;
    height: 20;

    Rectangle  {
        anchors {
            fill: parent;
            topMargin: parent.height / 3;
            bottomMargin: parent.height / 3;
        }

        radius: 8;

        gradient: Gradient  {
            GradientStop  {
                position: 0.0;
                color: "gray";
            }

            GradientStop  {
                position: 1.0;
                color: "white";
            }
        }

        z: -2;
    }

    Rectangle  {
        id: handleMinValue;

        x: (sliderMain.value.x - sliderMain.valueRange.x) * (sliderMain.width - handleMinValue.width) /
           (sliderMain.valueRange.y - sliderMain.valueRange.x);

        smooth: true;

        anchors.top: parent.top;

        width: 20;
        height: sliderMain.height;

        radius: parent.height / 2;

        Image {
            anchors.fill: parent;

            source: "qrc:/icons/left_arrow.svg";
        }

        MouseArea  {
            anchors.fill: parent;

            drag {
                axis: Drag.XAxis;
                target: parent;
                minimumX: 0;
                maximumX: sliderMain.width;
            }

            onPositionChanged:  {
                value.x = (sliderMain.valueRange.y - sliderMain.valueRange.x) * handleMinValue.x /
                        (sliderMain.width - handleMinValue.width) + sliderMain.valueRange.x;
            }
        }
    }

    Rectangle  {
        id: handleMaxValue;

        x: (sliderMain.value.y - sliderMain.valueRange.x) * (sliderMain.width - handleMaxValue.width) /
           (sliderMain.valueRange.y - sliderMain.valueRange.x);

        smooth: true;

        anchors.top: parent.top;

        width: 20;
        height: sliderMain.height;

        radius: parent.height / 2;

        Image {
            anchors.fill: parent;

            source: "qrc:/icons/right_arrow.svg";
        }

        MouseArea  {
            anchors.fill: parent;

            drag {
                axis: Drag.XAxis;
                target: parent;
                minimumX: 0;
                maximumX: sliderMain.width;
            }

            onPositionChanged:  {
                value.y = (sliderMain.valueRange.y - sliderMain.valueRange.x) * handleMaxValue.x /
                        (sliderMain.width - handleMinValue.width) + sliderMain.valueRange.x;
            }
        }
    }

    Rectangle {
        anchors {
            top: parent.top;
            bottom: parent.bottom;
            topMargin: parent.height / 3;
            bottomMargin: parent.height / 3;

            left: handleMinValue.right;
            right: handleMaxValue.left;
        }

        gradient: Gradient  {
            GradientStop  {
                position: 0.0;
                color: "cyan";
            }

            GradientStop  {
                position: 1.0;
                color: "blue";
            }
        }

        z: -1;
    }

    onValueChanged: clampValues();
    onValueRangeChanged: clampValues();

    function clampValues() {
        if (valueRange.x !== valueRange.y) {
            value.x = Helpers.clamp(value.x, valueRange.x, value.y);
            value.y = Helpers.clamp(value.y, value.x, valueRange.y);
        }
    }
}
