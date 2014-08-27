import QtQuick 2.3

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

    property variant model: null;

    Text {
        anchors.verticalCenter: parent.verticalCenter;
        x: 15;
        font.pixelSize: 14;
        text: parent.text;
    }

    Image {
        id: icon
        width: 20;
        height: 20;

        anchors {
            right: parent.right;
            rightMargin: 15;
            topMargin: 5;
            verticalCenter: parent.verticalCenter;
        }

        source: collapsed ? "qrc:/icons/expand.svg" : "qrc:/icons/collapse.svg";
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: if (mouse.button === Qt.LeftButton) {
                       parent.collapsed = !parent.collapsed;
                       //model.setProperty(index, "collapsed", !collapsed);
                   }
    }
}
