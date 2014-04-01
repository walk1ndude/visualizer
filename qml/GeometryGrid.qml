import QtQuick 2.0
import QtQuick.Controls 1.1

Grid {
    id: hudRotation
    columns: 3
    rows: 4
    spacing: 5

    signal angleChanged(real xRot, real yRot, real zRot);
    signal distChanged(real value);

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
        id: distText
        text: qsTr("dist")
    }

    Slider {
        id: distSlider
        width: 200
        minimumValue: 2.0
        maximumValue: 20.0
        value: 2.0
        onValueChanged: hudRotation.distChanged(value);
    }

    Text {
        id: disrSliderValue
        text: Math.round(distSlider.value * 1000) / 1000
    }

    function updateAngle() {
        angleChanged(xRotSlider.value, yRotSlider.value, zRotSlider.value);
    }
}
