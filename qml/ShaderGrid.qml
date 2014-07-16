import QtQuick 2.3
import QtQuick.Controls 1.2

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

        anchors.top: shaderGrid.top
        anchors.left: shaderGrid.left
        anchors.margins: 5

        Text {
            text: qsTr("minHU")
        }

        Slider {
            id: minHUSlider
            width: 200
            minimumValue: shaderGrid.huRange.x
            maximumValue: shaderGrid.huRange.y
            value: 400
            stepSize: 1.0
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
            stepSize: 1.0
        }

        Text {
            text: maxHUSlider.value
        }
    }

    Range {
        id: xRange
        objectName: "XRange"

        onRangeVectorChanged: shaderGrid.xRange = rangeVector

        anchors.top: grid.bottom
        anchors.left: shaderGrid.left
        anchors.margins: 5
    }


    Range {
        id: yRange
        objectName: "YRange"

        onRangeVectorChanged: shaderGrid.yRange = rangeVector

        anchors.top: xRange.bottom
        anchors.left: shaderGrid.left
        anchors.margins: 5
    }


    Range {
        id: zRange
        objectName: "ZRange"

        onRangeVectorChanged: shaderGrid.zRange = rangeVector

        anchors.top: yRange.bottom
        anchors.left: shaderGrid.left
        anchors.margins: 5
    }
}
