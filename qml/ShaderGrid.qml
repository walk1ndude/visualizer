import QtQuick 2.3

Rectangle {
    id: shaderGrid

    property vector2d xRange: xRange.rangeVector
    property vector2d yRange: yRange.rangeVector
    property vector2d zRange: zRange.rangeVector

    property vector2d huRange: Qt.vector2d(0, 65535)

    property int minHU: minHUSlider.value
    property int maxHU: maxHUSlider.value

    color: "#cccccc"
    border.color: "black"
    border.width: 2

    width: 200
    height: 200

    Grid {
        id: grid

        columns: 3
        rows: 6

        anchors {
            top: shaderGrid.top
            left: shaderGrid.left
            margins: 5
        }

        Text {
            text: qsTr("minHU")
        }

        Slider {
            id: minHUSlider
            width: 200
            minimumValue: shaderGrid.huRange.x
            maximumValue: shaderGrid.huRange.y
            value: 400
            //stepSize: 1.0
        }

        Text {
            text: minHUSlider.value
        }

        Text {
            text: qsTr("maxHU")
        }

        Slider {
            id: maxHUSlider
            width: 200
            minimumValue: shaderGrid.huRange.x
            maximumValue: shaderGrid.huRange.y
            value: 63512
            //stepSize: 1.0
        }

        Text {
            text: maxHUSlider.value
        }
    }

    Grid {
        columns: 2
        rows: 3

        anchors {
            top: grid.bottom
            left: shaderGrid.left
            margins: 15
        }

        Text {
            text: qsTr("xRange")
        }

        RangeSlider {
            id: xRange

            value: Qt.vector2d(-1.0, 1.0)
            valueRange: Qt.vector2d(-1.0, 1.0)

            onValueChanged: shaderGrid.xRange = value
        }

        Text {
            text: qsTr("yRange")
        }

        RangeSlider {
            id: yRange

            value: Qt.vector2d(-1.0, 1.0)
            valueRange: Qt.vector2d(-1.0, 1.0)

            onValueChanged: shaderGrid.yRange = value
        }

        Text {
            text: qsTr("zRange")
        }

        RangeSlider {
            id: zRange

            value: Qt.vector2d(-1.0, 1.0)
            valueRange: Qt.vector2d(-1.0, 1.0)

            onValueChanged: shaderGrid.zRange = value
        }
    }
}
