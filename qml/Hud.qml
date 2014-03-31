import QtQuick 2.0

Rectangle {
    id: hud
    color: "#71888888"
    width: 640
    height: 200
    focus: true

    signal rBottomChanged(real value);
    signal rTopChanged(real value);

    signal xRotChanged(real value);
    signal yRotChanged(real value);
    signal zRotChanged(real value);
    signal distChanged(real value);

    Row {
        y: 10
        spacing: 100

    Grid {
        id: hudShader
        columns: 2
        rows: 2
        spacing: 15

        Text {
            id: rBottomText
            text: qsTr("rBottom")
        }

        Slider {
            id: rBottomSlider
            width: 200
            position: 100
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
            position: 100
            coeff: 0.00390625
            onValueChanged: {
                rTopChanged(value);
            }
        }
    }

    Grid {
        id: hudRotation
        columns: 2
        rows: 4
        spacing: 15

        Text {
            id: xRotText
            text: qsTr("x axis")
        }

        Slider {
            id: xRotSlider
            width: 200
            minimum: -180
            maximum: 180
            onValueChanged: {
                xRotChanged(value);
            }
        }

        Text {
            id: yRotText
            text: qsTr("y axis")
        }

        Slider {
            id: yRot
            width: 200
            minimum: -180
            maximum: 180
            onValueChanged: {
                yRotChanged(value);
            }
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
            onValueChanged: {
                zRotChanged(value);
            }
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
                zRotChanged(value);
            }
        }
    }
    }
}
