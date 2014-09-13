import QtQuick 2.3

import "../js/helpers.js" as Helpers

Rectangle {
    id: geometryGrid;

    property real angleShot: 0.0;

    property int modelId: -1;

    property vector3d angle: Qt.vector3d(0.0, 0.0, 0.0);
    property real zoomFactor: 2.0;

    color: "#cccccc";
    border.color: "black";
    border.width: 2;

    width: 200;
    height: 200;
/*
    CircleSlider {
        id: circle;
    }
*/
    Grid {
        columns: 3;
        rows: 4;
        spacing: 5;

        anchors.top: circle.bottom;//geometryGrid.top;
        anchors.left: geometryGrid.left;
        anchors.margins: 10;

        Text {
            text: qsTr("x axis");
        }

        Slider {
            id: xRotSlider;
            width: 200;
            minimumValue: -180.0;
            maximumValue: 180.0;
            value: 0.0;
            onValueChanged: updateAngle();
        }

        Text {
            text: Helpers.pad((Math.round(xRotSlider.value * 1000) / 1000).toFixed(4), 3, 4);
        }

        Text {
            text: qsTr("y axis");
        }

        Slider {
            id: yRotSlider;
            width: 200;
            minimumValue: -180.0;
            maximumValue: 180.0;
            value: 0.0;
            onValueChanged: updateAngle();
        }

        Text {
            text: Helpers.pad((Math.round(yRotSlider.value * 1000) / 1000).toFixed(4), 3, 4);
        }

        Text {
            text: qsTr("z axis");
        }

        Slider {
            id: zRotSlider;
            width: 200;
            minimumValue: -180.0;
            maximumValue: 180.0;
            value: 0.0;
            onValueChanged: updateAngle();
        }

        Text {
            text: Helpers.pad((Math.round(zRotSlider.value * 1000) / 1000).toFixed(4), 3, 4);
        }

        Text {
            text: qsTr("zoom");
        }

        Slider {
            id: zoomSlider;
            width: 200;
            minimumValue: 0.1;
            maximumValue: 4.0;
            value: 2.0;
            onValueChanged: geometryGrid.zoomFactor = maximumValue - value;
        }

        Text {
            text: Helpers.pad((Math.round(zoomSlider.value * 1000) / 1000).toFixed(4), 3, 4);
        }
    }

    onAngleShotChanged: yRotSlider.value = geometryGrid.angleShot;

    function updateAngle() {
        geometryGrid.angle = Qt.vector3d(xRotSlider.value, yRotSlider.value, zRotSlider.value);
    }
}
