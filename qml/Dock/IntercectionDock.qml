import QtQuick 2.3


Rectangle {
    id: consoleWrapper;

    height: consoleOutput.head.height;

    Sidedock {
        id: consoleOutput;

        heading: qsTr("Console output");

        state: "bottom";

        color: "#880000FF";
    }
}
