import QtQuick 2.2
import QtQuick.Controls 1.1

Item {
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
            onValueChanged: hud.minValueChanged(value);
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
            onValueChanged: hud.maxValueChanged(value);
        }

        Text {
            text: maxValueSlider.value
        }
    }

    Range {
        id: sRange
        objectName: "SRange"

        onRangeVectorChanged: hud.sRange = rangeVector

        anchors.top: grid.bottom
    }


    Range {
        id: tRange
        objectName: "TRange"

        onRangeVectorChanged: hud.tRange = rangeVector

        anchors.top: sRange.bottom
    }


    Range {
        id: pRange
        objectName: "PRange"

        onRangeVectorChanged: hud.pRange = rangeVector

        anchors.top: tRange.bottom
    }
}
