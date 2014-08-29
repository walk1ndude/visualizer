import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

import ParserTools 1.0

import QtQuick.LocalStorage 2.0
import "../js/settings.js" as Settings

ApplicationWindow {
    id: appWindow;
    width: 1100;
    height: 768;

    property real sideBarWidth: 0.35

    title: "visualizer";

    color: "black";

    signal distsUpdated(variant distanses);
    signal pointUpdated(variant point);

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
        onAccepted: dicomReader.dicomFile = fileUrl;
    }

    FileDialog {
        id: openFileDialogReconstructor;
        title: "Choose image files";
        selectMultiple: true;
        onAccepted: reconstructor.imgFiles = fileUrls;
    }

    FileDialog {
        id: openFileDialogStl;
        title: "Choose stl files";
        onAccepted: stlReader.stlFile = fileUrl;
    }

    FocusScope {
        focus: true;
        anchors.fill: parent;

        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_Escape: Qt.quit(); break;
                case Qt.Key_Left: previousSlice(); break;
                case Qt.Key_Right: nextSlice(); break;
            }
        }
    }

    Row {
        id: modelRow;

        ModelViewerEx {
            id: modelViewer;
            width: appWindow.width - sidebar.width;
            height: appWindow.height - consoleDock.height;

            xRange: sidebar.xRange;
            yRange: sidebar.yRange;
            zRange: sidebar.zRange;

            minHU: sidebar.minHU;
            maxHU: sidebar.maxHU;

            rotation: sidebar.angle;
            zoomFactor: sidebar.zoomFactor;

            selectedPointName: sidebar.selectedPointName;
            selectedPointColor: sidebar.selectedPointColor;

            onPointUpdated: {
                sidebar.updateIndividualInfo();
                appWindow.pointUpdated(point);
            }
        }
    }

    SideBar {
        id: sidebar;
        anchors {
            left: modelRow.right;
            top: appWindow.top;
            bottom: appWindow.bottom;
            right: appWindow.right;
        }

        sidebarWidth: appWindow.width * appWindow.sideBarWidth;

        modelID: modelViewer.modelID;

        onDistsUpdated: appWindow.distsUpdated({"modelID": modelID, "dists": Settings.Distances[modelID]});
    }

    StlReader {
        id: stlReader;

        onModelRead: modelViewer.modelRead(buffers);
    }

    function nextSlice() {
        dicomReader.nextSlice(1);
        reconstructor.nextSlice(1);
    }

    function previousSlice() {
        dicomReader.nextSlice(-1);
        reconstructor.nextSlice(-1);
    }

    DicomReader {
        id: dicomReader;

        onSlicesProcessed: modelViewer.drawSlices(slices);
    }

    Reconstructor {
        id: reconstructor;

        onSlicesProcessed: modelViewer.drawSlices(slices);
    }

    Console {
        id: consoleDock;

        anchors {
            left: parent.left;
            right: sidebar.left;
            top: modelRow.bottom;
        }
    }
}
