import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

ApplicationWindow {
    id: appWindow
    width: 1000
    height: 670

    title: "visualizer"

    color: "black"

    signal fileOpenedDcm(url fileName);
    signal filesOpened(variant fileName);
    signal fileOpenedStl(url fileName);

    signal sliceNumberChanged(int ds);

    menuBar: MenuBar {
        id: menubar
        Menu {
            title: 'File'

            MenuItem {
                text: 'Open dicom'
                onTriggered: {
                    openFileDialogDicom.visible = true
                }
            }

            MenuItem {
                text: 'Open reconstructor'
                onTriggered: {
                    openFileDialogReconstructor.visible = true
                }
            }

            MenuItem {
                text: 'Open Stl file'
                onTriggered: {
                    openFileDialogStl.visible = true
                }
            }

            MenuItem {
                text: 'Quit'
                onTriggered: Qt.quit()
            }
        }
    }

    FileDialog {
        id: openFileDialogDicom
        title: 'Choose DICOM file'
        onAccepted: fileOpenedDcm(fileUrl);
    }

    FileDialog {
        id: openFileDialogReconstructor
        title: 'Choose image files'
        selectMultiple: true
        onAccepted: filesOpened(fileUrls);
    }

    FileDialog {
        id: openFileDialogStl
        title: 'Choose stl files'
        onAccepted: fileOpenedStl(fileUrl);
    }

    FocusScope {
        focus: true
        anchors.fill: parent

        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_Escape: Qt.quit(); break;
                case Qt.Key_Left: sliceNumberChanged(-1); break;
                case Qt.Key_Right: sliceNumberChanged(1); break;
                case Qt.Key_S: modelItem.takeShot = true; break;
            }
        }
    }

    Row {
        id: modelRow
        objectName: "modelRow"
        ModelItem {
            id: modelItem
            width: appWindow.width * 0.65
            height: appWindow.height
        }
    }

    SideBar {
        id: sidebar
        color: "#FFFFFF"

        width: appWindow.width - modelRow.width
        height: appWindow.height

        anchors.left: modelRow.right;

        onXRangeChanged: modelItem.xRange = xRange;
        onYRangeChanged: modelItem.yRange = yRange;
        onZRangeChanged: modelItem.zRange = zRange;

        onMinHUChanged: modelItem.minHU = minHU;
        onMaxHUChanged: modelItem.maxHU = maxHU;

        onAngleChanged: modelItem.rotation = angle;
        onZoomFactorChanged: modelItem.zoomFactor = zoomFactor;
    }
}
