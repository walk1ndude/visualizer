import QtQuick 2.2
import QtQuick.Controls 1.1

Grid {
    columns: 3
    rows: 3
    spacing: 5

    Text {
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
        text: hud.pad((Math.round(rBottomSlider.value * 1000) / 1000).toFixed(4), 3, 4)
    }

    Text {
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
        text: hud.pad((Math.round(rTopSlider.value * 1000) / 1000).toFixed(4), 3, 4)
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
