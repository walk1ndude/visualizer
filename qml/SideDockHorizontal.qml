import QtQuick 2.3

Rectangle {
    height: heading.height;

    property string heading: "";

    Heading {
        id: heading;
        text: parent.heading;

        property int parentY: 0;

        anchors {
            left: parent.left;
            right: parent.right;
        }

        onCollapsedChanged: {
            parent.height = collapsed ? height : height + 300;

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
