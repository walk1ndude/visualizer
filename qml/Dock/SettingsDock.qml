import QtQuick 2.3

Sidedock {
    id: settingsDock;

    heading: qsTr("Parameters");

    state: "top";

    Rectangle {
        anchors {
            left: parent.left;
            right: parent.right;
        }
    }
}

