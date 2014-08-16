import QtQuick 2.3

Item {
    id: viewPortHud
    property string text: "";

    property color color: "green"

    Rectangle {
        color: "transparent";

        anchors.fill: parent

        border {
            color: viewPortHud.color;
        }

        Text {
            id: text;
            text: viewPortHud.text;

            anchors {
                left: parent.left;
                bottom: parent.bottom;

                bottomMargin: - 0.8 * text.height;
                leftMargin: 0.2 * text.height;
            }

            color: viewPortHud.color;

            font {
                capitalization: Font.SmallCaps;
                pointSize: 24;
            }

            transform: Rotation {
                angle: -180;

                axis {
                    x: 1.0;
                    y: 0.0;
                    z: 0.0;
                }

                origin {
                    x: parent.x;
                    y: parent.y;
                }
            }
        }
    }
}
