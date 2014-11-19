import QtQuick 2.3;

import "qrc:/qml/Control" as Control;

Rectangle {
    id: modelSpecs;

    property vector2d xRange: xRange.rangeVector;
    property vector2d yRange: yRange.rangeVector;
    property vector2d zRange: zRange.rangeVector;

    property vector2d huRangeMax: Qt.vector2d(0, 65535);
    property vector2d huRangeModel: Qt.vector2d(0, 65535);

    property int windowCenterModel: 400;
    property int windowWidthModel: 1000;

    property int windowCenter: windowCenterSlider.value;
    property int windowWidth: windowWidthSlider.value;

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
        rows: 12;

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
            minimumValue: modelSpecs.huRangeMax.x;
            maximumValue: modelSpecs.huRangeMax.y;
            value: modelSpecs.huRangeModel.x;
            stepSize: 100.0;
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
            minimumValue: modelSpecs.huRangeMax.x;
            maximumValue: modelSpecs.huRangeMax.y;
            value: modelSpecs.huRangeModel.y;
            stepSize: 1.0;
        }

        Text {
            text: maxHUSlider.value;
        }

        Text {
            text: qsTr("windowCenter");
        }

        Control.Slider {
            id: windowCenterSlider;
            width: 200;
            minimumValue: 0;
            maximumValue: 10000;
            value: modelSpecs.windowCenterModel;
            stepSize: 100.0;
        }

        Text {
            text: windowCenterSlider.value;
        }

        Text {
            text: qsTr("windowWidth");
        }

        Control.Slider {
            id: windowWidthSlider;
            width: 200;
            minimumValue: 0;
            maximumValue: 10000;
            value: modelSpecs.windowWidthModel;
            stepSize: 100.0;
        }

        Text {
            text: windowWidthSlider.value;
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
