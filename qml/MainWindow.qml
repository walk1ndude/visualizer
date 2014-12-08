import QtQuick 2.3;
import QtQuick.Controls 1.2;
import QtQuick.Dialogs 1.2;

import RenderTools 1.0;

import "qrc:/js/settings.js" as Settings;

import "Dock" as Dock;
import "Viewer" as Viewer;

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
        onAccepted: {
            var component = Qt.createComponent("Parser/DicomReaderEx.qml");
            var dicomReader = component.createObject(null, {
                                                       "viewer" : modelViewer
                                                   });
            dicomReader.file = fileUrl;
        }
    }

    FileDialog {
        id: openFileDialogReconstructor;
        title: "Choose image files";
        selectMultiple: true;
        onAccepted: {
            var component = Qt.createComponent("Parser/ReconstructorEx.qml");
            var reconstructor = component.createObject(null, {
                                                           "viewer" : modelViewer
                                                       });
            reconstructor.files = fileUrls;
        }
    }

    FileDialog {
        id: openFileDialogStl;
        title: "Choose stl files";
        onAccepted: {
            var component = Qt.createComponent("Parser/StlReaderEx.qml");
            var stlReader = component.createObject(null, {
                                                       "viewer" : modelViewer
                                                   });
            stlReader.file = fileUrl;
       }
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

        Viewer.ModelViewerEx {
            id: modelViewer;
            width: appWindow.width - sidebar.width;
            height: appWindow.height - consoleDock.height;

            onPointUpdated: {
                sidebar.updateIndividual();
                appWindow.pointUpdated(point);
            }

            onPost: {
                switch (message.header.reciever) {
                    case "sidebar" :
                        sidebar.recieve(message);
                        break;
                    }
            }
        }
    }

    onPointUpdated: sidebar.updatePoint(point);

    Dock.Sidebar {
        id: sidebar;

        anchors {
            left: modelRow.right;
            top: appWindow.top;
            bottom: appWindow.bottom;
            right: appWindow.right;
        }

        onPost: modelViewer.message = message;

        dX: appWindow.width * appWindow.sideBarWidth;

        //onDistsUpdated: appWindow.distsUpdated({"modelID": modelID, "dists": Settings.Distances[modelID]});
    }

    function nextSlice() {
        translateBy(1);
    }

    function previousSlice() {
        translateBy(-1);
    }

    function translateBy(slicer, dS) {
        if (!!dicomReader) {
            dicomReader.nextSlice(dS);
        }
        if (!!reconstructor) {
            reconstructor.nextSlice(dS);
        }
    }

    function toggleDocks() {
        sidebar.head.collapsed = false;
    }

    Dock.ConsoleDock {
        id: consoleDock;

        anchors {
            left: parent.left;
            right: sidebar.left;
            top: modelRow.bottom;
        }
    }
}
