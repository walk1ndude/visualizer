import QtQuick 2.2
import QtQuick.Controls 1.1

Grid {
    id: hudShader
    columns: 3
    rows: 3
    spacing: 5

    Text {
        id: rBottomText
        text: qsTr("rBottom")
    }

    Slider {
        id: rBottomSlider
        width: 200
        minimumValue: 0.0
        maximumValue: 1.0
        value: 0.2
        onValueChanged: hud.rBottomChanged(value);
    }

    Text {
        id: rBottomSliderValue
        text: Math.round(rBottomSlider.value * 1000) / 1000
    }

    Text {
        id: rTopText
        text: qsTr("rTop")
    }

    Slider {
        id: rTopSlider
        width: 200
        minimumValue: 0.0
        maximumValue: 1.0
        value: 0.6
        onValueChanged: hud.rTopChanged(value);
    }

    Text {
        id: rTopSliderValue
        text: Math.round(rTopSlider.value * 1000) / 1000
    }

    Text {
        id: ambientIntensityText
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
        id: ambientIntensitySliderValue
        text: Math.round(ambientIntensitySlider.value * 1000) / 1000
    }
}
