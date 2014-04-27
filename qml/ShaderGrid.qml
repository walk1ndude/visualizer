import QtQuick 2.2
import QtQuick.Controls 1.1

Item {
    id: shaderGrid

    property vector2d sRange: Qt.vector2d(0.0, 1.0)
    property vector2d tRange: Qt.vector2d(0.0, 1.0)
    property vector2d pRange: Qt.vector2d(0.0, 1.0)

    property int minValue: 5
    property int maxValue: 100

    Grid {
        id: grid

        columns: 3
        rows: 6
        spacing: 5

        Text {
            text: qsTr("minValue")
        }

        Slider {
            id: minValueSlider
            width: 200
            minimumValue: 0
            maximumValue: 255
            value: 5
            stepSize: 1.0
            onValueChanged: shaderGrid.minValue = value;
        }

        Text {
            text: minValueSlider.value
        }

        Text {
            text: qsTr("maxValue")
        }

        Slider {
            id: maxValueSlider
            width: 200
            minimumValue: 0
            maximumValue: 255
            value: 100
            stepSize: 1.0
            onValueChanged: shaderGrid.maxValue = value;
        }

        Text {
            text: maxValueSlider.value
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
