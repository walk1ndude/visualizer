import QtQuick 2.3

import "../js/helperfuncs.js" as HelperFuncs

Grid {
    id: range

    property vector2d rangeVector: Qt.vector2d(
                                       Math.min(minRangeSlider.value, maxRangeSlider.value),
                                       Math.max(minRangeSlider.value, maxRangeSlider.value)
                                       )

    columns: 3
    rows: 2
    spacing: 5

    Text {
        text: qsTr("min" + range.objectName)
    }

    Slider {
        id: minRangeSlider
        width: 200
        minimumValue: -1.0
        maximumValue: 1.0
        value: -1.0
    }

    Text {
        text: HelperFuncs.pad((Math.round(minRangeSlider.value * 1000) / 1000).toFixed(4), 3, 4)
    }

    Text {
        text: qsTr("max" + range.objectName)
    }

    Slider {
        id: maxRangeSlider
        width: 200
        minimumValue: -1.0
        maximumValue: 1.0
        value: 1.0
    }

    Text {
        text: HelperFuncs.pad((Math.round(maxRangeSlider.value * 1000) / 1000).toFixed(4), 3, 4)
    }
}
