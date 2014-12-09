import QtQuick 2.3;

Rectangle {
    border {
        color: "black";
        width: 5;
    }

    color: "white";

    height: 50;
    width: 20;

    property string text: "";
    property bool collapsed: true;

    property var model: null;

    property url collapsedIcon: "qrc:/icons/collapse.svg";
    property url uncollapsedIcon: "qrc:/icons/expand.svg";

    Text {
        anchors.verticalCenter: parent.verticalCenter;
        x: 15;
        font.pixelSize: 14;
        text: parent.text;
    }

    Image {
        id: icon;
        width: 20;
        height: 20;

        anchors {
            right: parent.right;
            rightMargin: 15;
            topMargin: 5;
            verticalCenter: parent.verticalCenter;
        }

        source: collapsed ? parent.collapsedIcon : parent.uncollapsedIcon;
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: if (mouse.button === Qt.LeftButton) {
                       parent.collapsed = !parent.collapsed;
                   }
    }
}
