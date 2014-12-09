import QtQuick 2.3;

Rectangle {
    id: sidedock;
    height: heading.height;

    property string heading: "";
    property var head: heading;

    // means where heading goes: below or above dock content, true - below
    property bool inverseFolding: true;

    property real dX: 300;
    property real dY: 300;

    states: [
        State {
            name: "horizontal";

            AnchorChanges {
                target: sidedock;

                anchors {
                    left: parent.left;
                    right: parent.right;
                }
            }

            AnchorChanges {
                target: heading;

                anchors {
                    left: parent.left;
                    right: parent.right;
                }
            }

            PropertyChanges {
                target: sidedock;
                height: heading.height;
            }
        },

        State {
            name: "vertical";

            PropertyChanges {
                target: heading;
                width: parent.height;
                transform: [
                    translateTo,
                    rotate,
                    translateBack
                ]
            }

            PropertyChanges {
                target: sidedock;
                width: heading.height;
                height: parent ? parent.height : parent.dX;
            }
        }
    ]

    state: "horizontal";

    Translate {
        id: translateTo;
        x: - width / 2;
        y: - height / 2;
    }

    Rotation {
        id: rotate;
        angle: 90;

        axis {
            x: 0;
            y: 0;
            z: 1;
        }
    }

    Translate {
        id: translateBack;
        x: height / 2 + width - height;
        y: width / 2;
    }

    color: "#AAFFFFFF";

    onDXChanged: heading.recalcWidth();
    onDYChanged: heading.recalcHeight();

    Heading {
        id: heading;
        text: parent.heading;

        property int parentX: 0;
        property int parentY: 0;

        function recalcWidth() {
            parent.width = collapsed ? height : height + dX;

            x = parentX < parent.x ? (inverseFolding  ? 1 : -1) * (parent.width - height) : 0;

            parentX = parent.x;
        }

        function recalcHeight() {
            parent.height = collapsed ? height : height + dY;

            parent.y = parentY <= parent.y ? (inverseFolding  ? 1 : -1) * (height - parent.height) : 0;
            y = (inverseFolding  ? 1 : -1) * (parent.height - height);

            parentY = parent.y;
        }

        onCollapsedChanged: {
            parent.state === "horizontal" ? recalcHeight() : recalcWidth();
        }
    }

    onXChanged: {
        if (!!heading.parentX) {
            heading.parentX = parent.x;
        }
    }

    onYChanged: {
        if (!!heading.parentY) {
            heading.parentY = parent.y;
        }
    }
}
