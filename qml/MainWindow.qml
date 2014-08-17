import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

import QtQuick.LocalStorage 2.0
import "../js/settings.js" as Settings

ApplicationWindow {
    id: appWindow;
    width: 1100;
    height: 768;

    property real sideBarWidth: 0.65

    title: "visualizer";

    color: "black";

    signal filesOpened(variant fileName);
    signal fileOpenedStl(url fileName);

    signal sliceNumberChanged(int ds);

    signal distsUpdated(variant distanses);

    menuBar: MenuBar {
        id: menubar;
        Menu {
            title: "File";

            MenuItem {
                text: "Open dicom";
                onTriggered: {
                    openFileDialogDicom.visible = true;
                }
            }

            MenuItem {
                text: "Open reconstructor";
                onTriggered: {
                    openFileDialogReconstructor.visible = true;
                }
            }

            MenuItem {
                text: "Open Stl file";
                onTriggered: {
                    openFileDialogStl.visible = true;
                }
            }

            MenuItem {
                text: "Quit";
                onTriggered: Qt.quit();
            }
        }
    }

    FileDialog {
        id: openFileDialogDicom;
        title: "Choose DICOM file";
        onAccepted: modelViewer.dicomFile = fileUrl;
    }

    FileDialog {
        id: openFileDialogReconstructor;
        title: "Choose image files";
        selectMultiple: true;
        onAccepted: filesOpened(fileUrls);
    }

    FileDialog {
        id: openFileDialogStl;
        title: "Choose stl files";
        onAccepted: fileOpenedStl(fileUrl);
    }

    FocusScope {
        focus: true;
        anchors.fill: parent;

        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_Escape: Qt.quit(); break;
                case Qt.Key_Left: modelViewer.previousSlide(); break;
                case Qt.Key_Right: modelViewer.nextSlide(); break;
            }
        }
    }

    Row {
        id: modelRow;
        objectName: "modelRow";
        ModelViewerEx {
            id: modelViewer;
            width: appWindow.width * sideBarWidth;
            height: appWindow.height;

            xRange: sidebar.xRange;
            yRange: sidebar.yRange;
            zRange: sidebar.zRange;

            minHU: sidebar.minHU;
            maxHU: sidebar.maxHU;

            rotation: sidebar.angle;
            zoomFactor: sidebar.zoomFactor;

            selectedPointName: sidebar.selectedPointName;
            selectedPointColor: sidebar.selectedPointColor;

            onUpdateIndividualInfo: sidebar.updateIndividualInfo();
        }
    }

    SideBar {
        id: sidebar;
        color: "#FFFFFF";

        modelID: modelViewer.modelID;

        width: appWindow.width - modelRow.width;
        height: appWindow.height;

        anchors.left: modelRow.right;

        onDistsUpdated: appWindow.distsUpdated({"modelID": modelID, "dists": Settings.Distances[modelID]});
    }
}
