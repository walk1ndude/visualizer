import QtQuick 2.3

Rectangle {
    height: heading.height;

    property string heading: "";

    property variant head: heading;

    property real dY: 300;

    Heading {
        id: heading;
        text: parent.heading;

        property int parentY: 0;

        anchors {
            left: parent.left;
            right: parent.right;
        }

        onCollapsedChanged: {
            parent.height = collapsed ? height : height + dY;

            y = parentY < parent.y ? parent.height - height : 0;

            parentY = parent.y;
        }
    }

    onYChanged: {
        if (!!heading.parentY) {
            heading.parentY = parent.y;
        }

    }

    color: "#AAFFFFFF";
}
