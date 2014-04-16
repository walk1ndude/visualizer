import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1

ApplicationWindow {
    width: 640
    height: 670

    signal fileOpened(string fileName);
    signal sliceNumberChanged(int ds);

    menuBar: MenuBar {
        Menu {
            title: '<font color="black">File</font>'
            MenuItem {
                text: '<font color="black">Open</font>'
                onTriggered: {
                    openFileDialog.visible = true
                }
            }
            MenuItem {
                text: '<font color="black">Quit</font>'
                onTriggered: Qt.quit()
            }
        }
    }

    FileDialog {
        id: openFileDialog
        title: "Choose DICOM file"
        selectMultiple: false
        selectFolder: false
        nameFilters: [ "DICOM files (*.dcm)" ]
        selectedNameFilter: "DICOM files (*.dcm)"
        onAccepted: {
            fileOpened(fileUrl);
        }
    }

    FocusScope {
        focus: true
        anchors.fill: parent

        Keys.onPressed: {
            switch (event.key) {
            case Qt.Key_Escape: Qt.quit(); break;
            case Qt.Key_Left: sliceNumberChanged(-1); break;
            case Qt.Key_Right: sliceNumberChanged(1); break;
            }
        }
    }

    SliceItem {
        objectName: "sliceItem"

        anchors.fill: parent
    }
}
