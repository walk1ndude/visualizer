import QtQuick 2.2
import QtQuick.Controls 1.1

Grid {
    columns: 3
    rows: 3
    spacing: 5

    Text {
        text: qsTr("minValue")
    }

    Slider {
        id: minValueSlider
        width: 200
        minimumValue: 0
        maximumValue: 150
        value: 40
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
        maximumValue: 150
        value: 55
        onValueChanged: hud.maxValueChanged(value);
    }

    Text {
        text: maxValueSlider.value
    }

    Text {
        text: qsTr("ambientIntensity")
    }

    Slider {
        id: ambientIntensitySlider
        width: 200
        minimumValue: 0.1
        maximumValue: 30.0
        value: 3.9
        onValueChanged: hud.ambientIntensityChanged(value);
    }

    Text {
        text: hud.pad((Math.round(ambientIntensitySlider.value * 1000) / 1000).toFixed(4), 3, 4)
    }
}
