import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1

ApplicationWindow {
    id: appWindow
    width: 640
    height: 670

    title: "sliceViewer"

    signal fileOpened(string fileName);
    signal sliceNumberChanged(int ds);

    menuBar: MenuBar {
        Menu {
            title: 'File'
            MenuItem {
                text: 'Open'
                onTriggered: {
                    openFileDialog.visible = true
                }
            }
            MenuItem {
                text: 'Quit'
                onTriggered: Qt.quit()
            }
        }
    }

    FileDialog {
        id: openFileDialog
        title: 'Choose DICOM file'
        onAccepted: fileOpened(fileUrl);
    }

    FocusScope {
        focus: true
        anchors.fill: parent

        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_Escape: Qt.quit(); break;
                case Qt.Key_Left: sliceNumberChanged(-1); break;
                case Qt.Key_Right: sliceNumberChanged(1); break;
                case Qt.Key_S: sliceItem.takeShot = true; break;
            }
        }
    }

    Row {
        objectName: "sliceRow"
        SliceItem {
            id: sliceItem
            width: appWindow.width
            height: appWindow.height
        }
    }
}
