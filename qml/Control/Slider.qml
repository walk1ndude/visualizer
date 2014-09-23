import QtQuick 2.3;

import "qrc:/js/helpers.js" as Helpers;

Item {
    id: sliderMain;

    property real minimumValue: 0.0;
    property real maximumValue: 180.0;
    property real value: 0.0;

    width: 200;
    height: 20;

    Rectangle  {
        anchors {
            verticalCenter: parent.verticalCenter;
            fill: parent;
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
    }

    Rectangle  {
        id: handle;

        x: (sliderMain.value - sliderMain.minimumValue) * (sliderMain.width - handle.width) / (sliderMain.maximumValue - sliderMain.minimumValue);

        smooth: true;

        anchors.top: parent.top;

        width: 20;
        height: sliderMain.height;

        radius: parent.height / 2;

        gradient: Gradient  {
            GradientStop  {
                position: 0.0;
                color: "lightgray";
            }
            GradientStop  {
                position: 1.0;
                color: "gray";
            }
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
                value = (sliderMain.maximumValue - sliderMain.minimumValue) * handle.x / (sliderMain.width - handle.width) + sliderMain.minimumValue;
            }
        }
    }

    onValueChanged: {
        value = Helpers.clamp(value, minimumValue, maximumValue);
    }
}
