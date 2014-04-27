import QtQuick 2.2

Item {
    id: hud

    signal minValueChanged(int value);
    signal maxValueChanged(int value);

    property vector3d angle: Qt.vector3d(0.0, 0.0, 0.0)
    property real zoomFactor: 2.0

    property vector2d sRange: Qt.vector2d(0.0, 1.0)
    property vector2d tRange: Qt.vector2d(0.0, 1.0)
    property vector2d pRange: Qt.vector2d(0.0, 1.0)

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
