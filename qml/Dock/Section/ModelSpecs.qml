import QtQuick 2.3;

import "qrc:/qml/Control" as Control;

Rectangle {
    id: modelSpecs;

    property vector2d xRange: xRange.rangeVector;
    property vector2d yRange: yRange.rangeVector;
    property vector2d zRange: zRange.rangeVector;

    property vector2d huRange: Qt.vector2d(0, 65535);

    property int minHU: minHUSlider.value;
    property int maxHU: maxHUSlider.value;

    color: "#cccccc";
    border.color: "black";
    border.width: 2;

    width: 200;
    height: 200;

    Grid {
        id: grid;

        columns: 3;
        rows: 6;

        anchors {
            top: modelSpecs.top;
            left: modelSpecs.left;
            margins: 5;
        }

        Text {
            text: qsTr("minHU");
        }

        Control.Slider {
            id: minHUSlider;
            width: 200;
            minimumValue: modelSpecs.huRange.x;
            maximumValue: modelSpecs.huRange.y;
            value: 400;
            //stepSize: 1.0;
        }

        Text {
            text: minHUSlider.value;
        }

        Text {
            text: qsTr("maxHU");
        }

        Control.Slider {
            id: maxHUSlider;
            width: 200;
            minimumValue: modelSpecs.huRange.x;
            maximumValue: modelSpecs.huRange.y;
            value: 63512;
            //stepSize: 1.0;
        }

        Text {
            text: maxHUSlider.value;
        }
    }

    Grid {
        columns: 2;
        rows: 3;

        anchors {
            top: grid.bottom;
            left: modelSpecs.left;
            margins: 15;
        }

        Text {
            text: qsTr("xRange");
        }

        Control.RangeSlider {
            id: xRange;

            value: Qt.vector2d(-1.0, 1.0);
            valueRange: Qt.vector2d(-1.0, 1.0);

            onValueChanged: modelSpecs.xRange = value;
        }

        Text {
            text: qsTr("yRange");
        }

        Control.RangeSlider {
            id: yRange;

            value: Qt.vector2d(-1.0, 1.0);
            valueRange: Qt.vector2d(-1.0, 1.0);

            onValueChanged: modelSpecs.yRange = value;
        }

        Text {
            text: qsTr("zRange");
        }

        Control.RangeSlider {
            id: zRange;

            value: Qt.vector2d(-1.0, 1.0);
            valueRange: Qt.vector2d(-1.0, 1.0);

            onValueChanged: modelSpecs.zRange = value;
        }
    }
}
