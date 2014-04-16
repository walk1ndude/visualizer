import QtQuick 2.2
import QtQuick.Controls 1.1

Grid {
    id: hudRotation
    columns: 3
    rows: 4
    spacing: 5

    Text {
        id: xRotText
        text: qsTr("x axis")
    }

    Slider {
        id: xRotSlider
        width: 200
        minimumValue: -180.0
        maximumValue: 180.0
        value: 0.0
        onValueChanged: updateAngle();
    }

    Text {
        id: xRotSliderValue
        text: Math.round(xRotSlider.value * 1000) / 1000
    }

    Text {
        id: yRotText
        text: qsTr("y axis")
    }

    Slider {
        id: yRotSlider
        width: 200
        minimumValue: -180.0
        maximumValue: 180.0
        value: 0.0
        onValueChanged: updateAngle();
    }

    Text {
        id: yRotSliderValue
        text: Math.round(yRotSlider.value * 1000) / 1000
    }

    Text {
        id: zRotText
        text: qsTr("z axis")
    }

    Slider {
        id: zRotSlider
        width: 200
        minimumValue: -180.0
        maximumValue: 180.0
        value: 0.0
        onValueChanged: updateAngle();
    }

    Text {
        id: zRotSliderValue
        text: Math.round(zRotSlider.value * 1000) / 1000
    }

    Text {
        id: zoomZText
        text: qsTr("zoom Z")
    }

    Slider {
        id: zoomZSlider
        width: 200
        minimumValue: 0.1
        maximumValue: 4.0
        value: 2.0
        onValueChanged: hud.zoomZChanged(value);
    }

    Text {
        id: zoomZSliderValue
        text: Math.round(zoomZSlider.value * 1000) / 1000
    }

    function updateAngle() {
        hud.angleChanged(xRotSlider.value, yRotSlider.value, zRotSlider.value);
    }
}
