import QtQuick 2.3;

import "qrc:/js/helpers.js" as Helpers;

Item {
    id: sliderMain;

    property real minimumValue: 0.0;
    property real maximumValue: 180.0;
    property real value: 0.0;

    property real stepSize: 1.0;

    width: 200;
    height: 20;

    Rectangle  {
        id: rangeRect;

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
        id: handle;

        x: (sliderMain.value - sliderMain.minimumValue) * (sliderMain.width - handle.width) / (sliderMain.maximumValue - sliderMain.minimumValue);

        smooth: true;

        anchors.top: parent.top;

        width: 20;
        height: sliderMain.height;

        radius: parent.height / 2;

        color: "black";

        MouseArea  {
            anchors.fill: parent;

            drag {
                axis: Drag.XAxis;
                target: parent;
                minimumX: 0;
                maximumX: sliderMain.width;
            }

            onPositionChanged:  {
                var newValue = Math.round((sliderMain.maximumValue - sliderMain.minimumValue) * handle.x / (sliderMain.width - handle.width) + sliderMain.minimumValue);
                value += (Math.ceil((newValue - value) / stepSize) * stepSize);
            }
        }
    }

    Rectangle {
        anchors {
            top: parent.top;
            bottom: parent.bottom;
            topMargin: parent.height / 3;
            bottomMargin: parent.height / 3;

            left: parent.left;
            right: handle.right;
        }

        radius: rangeRect.radius;

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

    onValueChanged: {
        value = Helpers.clamp(value, minimumValue, maximumValue);
    }
}
