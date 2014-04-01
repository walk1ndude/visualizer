import QtQuick 2.0

Grid {
    id: hudShader
    columns: 2
    rows: 2
    spacing: 15

    signal rBottomChanged(real value);
    signal rTopChanged(real value);

    Text {
        id: rBottomText
        text: qsTr("rBottom")
    }

    Slider {
        id: rBottomSlider
        width: 200
        position: 20
        coeff: 0.00390625
        onValueChanged: {
            rBottomChanged(value);
        }
    }

    Text {
        id: rTopText
        text: qsTr("rTop")
    }

    Slider {
        id: rTopSlider
        width: 200
        position: 160
        coeff: 0.00390625
        onValueChanged: {
            rTopChanged(value);
        }
    }
}
