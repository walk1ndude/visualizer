import QtQuick 2.3

Text {
    text: "";

    property real xNormalized: 0;
    property real yNormalized: 0;

    x: xNormalized * parent.width + 10;
    y: yNormalized * parent.height - 10;

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
