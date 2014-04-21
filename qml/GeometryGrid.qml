import QtQuick 2.2
import QtQuick.Controls 1.1

Grid {
    columns: 3
    rows: 4
    spacing: 5

    Text {
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
        text: hud.pad((Math.round(xRotSlider.value * 1000) / 1000).toFixed(4), 3, 4)
    }

    Text {
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
        text: hud.pad((Math.round(yRotSlider.value * 1000) / 1000).toFixed(4), 3, 4)
    }

    Text {
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
        text: hud.pad((Math.round(zRotSlider.value * 1000) / 1000).toFixed(4), 3, 4)
    }

    Text {
        text: qsTr("zoom")
    }

    Slider {
        id: zoomSlider
        width: 200
        minimumValue: 0.1
        maximumValue: 4.0
        value: 2.0
        onValueChanged: hud.zoomChanged(value);
    }

    Text {
        text: hud.pad((Math.round(zoomSlider.value * 1000) / 1000).toFixed(4), 3, 4)
    }

    function updateAngle() {
        hud.angleChanged(xRotSlider.value, yRotSlider.value, zRotSlider.value);
    }
}
