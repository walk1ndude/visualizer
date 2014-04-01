import QtQuick 2.0

Grid {
    id: hudRotation
    columns: 2
    rows: 4
    spacing: 15

    signal angleChanged(real xRot, real yRot, real zRot);
    signal distChanged(real value);

    Text {
        id: xRotText
        text: qsTr("x axis")
    }

    Slider {
        id: xRotSlider
        width: 200
        minimum: -180
        maximum: 180
        onValueChanged: updateAngle();
    }

    Text {
        id: yRotText
        text: qsTr("y axis")
    }

    Slider {
        id: yRotSlider
        width: 200
        minimum: -180
        maximum: 180
        onValueChanged: updateAngle();
    }

    Text {
        id: zRotText
        text: qsTr("z axis")
    }

    Slider {
        id: zRotSlider
        width: 200
        minimum: -180
        maximum: 180
        onValueChanged: updateAngle();
    }

    Text {
        id: distText
        text: qsTr("z axis")
    }

    Slider {
        id: distSlider
        width: 200
        coeff: 1.0
        value: 4.0
        minimum: 4
        maximum: 20
        onValueChanged: {
            distChanged(value);
        }
    }

    function updateAngle() {
        angleChanged(xRotSlider.value, yRotSlider.value, zRotSlider.value);
    }
}
