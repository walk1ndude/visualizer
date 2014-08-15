import QtQuick 2.3

Item {
    id: viewPortHud
    property string text: "";

    Rectangle {
        color: "transparent";

        anchors.fill: parent

        border {
            color: "green";
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


            color: "green";

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
