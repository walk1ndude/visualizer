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

    property var model: null;

    property url collapsedIcon: "qrc:/icons/collapse.svg";
    property url expandedIcon: "qrc:/icons/expand.svg";

    state: "collapsed";

    states: [
        State {
            name: "collapsed";
        },
        State {
            name: "expanded";
        }
    ]

    Text {
        anchors.verticalCenter: parent.verticalCenter;
        x: 15;
        font.pixelSize: 14;
        text: parent.text;
    }

    Image {
        id: icon;
        width: parent.height * 0.8;
        height: parent.height * 0.8;

        anchors {
            right: parent.right;
            rightMargin: 15;
            topMargin: 5;
            verticalCenter: parent.verticalCenter;
        }

        source: parent.state !== "collapsed" ? parent.collapsedIcon : parent.expandedIcon;
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: {
                if (mouse.button === Qt.LeftButton) {
                    parent.state = parent.state === "collapsed" ? "expanded" : "collapsed";
                }
        }
    }
}
