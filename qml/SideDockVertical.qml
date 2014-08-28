import QtQuick 2.3

Rectangle {
    width: heading.height;
    height: parent ? parent.height : 300;

    property string heading: "";

    property int dX: 300;

    Heading {
        id: heading;
        text: parent.heading;

        property int parentX: 0;

        width: parent.height;

        onCollapsedChanged: {
            parent.width = collapsed ? height : height + dX;
        }

        transform: [
            Translate {
                x: - width / 2;
                y: - height / 2;
            },

            Rotation {
                angle: 90;

                axis {
                    x: 0;
                    y: 0;
                    z: 1;
                }
            },

            Translate {
                x: height / 2 - parent.height + width;
                y: width / 2;
            }

        ]
    }

    onXChanged: {
        if (!!heading.parentX) {
            heading.parentX = parent.x;
        }
    }

    color: "#AAFFFFFF";
}
