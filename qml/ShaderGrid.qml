import QtQuick 2.2
import QtQuick.Controls 1.1

Item {
    id: shaderGrid

    property vector2d sRange: sRange.rangeVector
    property vector2d tRange: tRange.rangeVector
    property vector2d pRange: pRange.rangeVector

    property vector2d huRange: Qt.vector2d(0, 65535)

    property int minHU: minHUSlider.value
    property int maxHU: maxHUSlider.value

    Grid {
        id: grid

        columns: 3
        rows: 6
        spacing: 5

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
        id: sRange
        objectName: "SRange"

        onRangeVectorChanged: shaderGrid.sRange = rangeVector

        anchors.top: grid.bottom
    }


    Range {
        id: tRange
        objectName: "TRange"

        onRangeVectorChanged: shaderGrid.tRange = rangeVector

        anchors.top: sRange.bottom
    }


    Range {
        id: pRange
        objectName: "PRange"

        onRangeVectorChanged: shaderGrid.pRange = rangeVector

        anchors.top: tRange.bottom
    }
}
