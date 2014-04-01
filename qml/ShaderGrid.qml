import QtQuick 2.0
import QtQuick.Controls 1.1

Grid {
    id: hudShader
    columns: 3
    rows: 2
    spacing: 5

    signal rBottomChanged(real value);
    signal rTopChanged(real value);

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
        onValueChanged: hudShader.rBottomChanged(value);
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
        onValueChanged: hudShader.rTopChanged(value);
    }

    Text {
        id: rTopSliderValue
        text: Math.round(rTopSlider.value * 1000) / 1000
    }
}
