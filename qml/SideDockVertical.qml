import QtQuick 2.3

Rectangle {
    width: heading.height;
    height: parent ? parent.height : 300;

    property string heading: "";

    property int dX: 300;

    property variant head: heading;

    onDXChanged: heading.recalcWidth();

    Heading {
        id: heading;
        text: parent.heading;

        property int parentX: 0;

        function recalcWidth() {
            parent.width = collapsed ? height : height + dX;

            x = parentX < parent.x ? height - parent.width : 0;

            parentX = parent.x;
        }

        width: parent.height;

        onCollapsedChanged: recalcWidth();

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
                x: height / 2 + width - height;
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
