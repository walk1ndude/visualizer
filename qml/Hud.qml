import QtQuick 2.2

Item {
    id: hud

    signal minValueChanged(int value);
    signal maxValueChanged(int value);
    signal ambientIntensityChanged(real value);

    signal angleChanged(real xRot, real yRot, real zRot);
    signal zoomChanged(real value);

    Rectangle {
        color: "#71888888"

        anchors.fill: parent

        ShaderGrid {
            anchors.left: parent.left
        }

        GeometryGrid {
            anchors.right: parent.right
            anchors.rightMargin: 10
        }
    }

    function pad (number, max, decimal) {
        return (Array(max + 1).join(' ') + number).slice(-max - decimal - 2);
    }
}
