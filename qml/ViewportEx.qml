import QtQuick 2.3

import RenderTools 1.0

Viewport {
    id: viewport;

    property color color: "green";

    x: boundingRect.x * parent.width;
    y: boundingRect.y * parent.height;

    width: boundingRect.width * parent.width;
    height: boundingRect.height * parent.height;

    Rectangle {
        color: "transparent";

        anchors.fill: parent

        border {
            color: viewport.color;
        }

        Text {
            id: text;
            text: viewport.text;

            anchors {
                left: parent.left;
                bottom: parent.bottom;

                bottomMargin: - 0.8 * text.height;
                leftMargin: 0.2 * text.height;
            }

            color: viewport.color;

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
