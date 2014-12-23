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

    property real sidebarWidth: 0.35;
    property real consoleDockHeight: 0.4;

    title: "visualizer";

    color: "black";

    signal recieve(variant message);

    menuBar: MenuBar {
        id: menubar;
        Menu {
            title: qsTr("File");

            MenuItem {
                text: qsTr("Open dicom");
                onTriggered: {
                    openFileDialogDicom.visible = true;
                }
            }

            MenuItem {
                text: qsTr("Open reconstructor");
                onTriggered: {
                    openFileDialogReconstructor.visible = true;
                }
            }

            MenuItem {
                text: qsTr("Open Stl file");
                onTriggered: {
                    openFileDialogStl.visible = true;
                }
            }

            MenuItem {
                text: qsTr("Quit");
                onTriggered: Qt.quit();
            }
        }

        Menu {
            title: qsTr("Edit");

            MenuItem {
                text: qsTr("Find interceptions");
                onTriggered: {
                    openFileDialogDicom.visible = true;
                }
            }
        }
    }

    FileDialog {
        id: openFileDialogDicom;
        title: qsTr("Choose DICOM file");
        onAccepted: readDicom(fileUrl)
    }

    FileDialog {
        id: openFileDialogReconstructor;
        title: qsTr("Choose image files");
        selectMultiple: true;
        onAccepted: readReconstructor(fileUrls);
    }

    FileDialog {
        id: openFileDialogStl;
        title: qsTr("Choose stl files");
        onAccepted: readSTL(fileUrl);
    }

    function readDicom(fileUrl) {
        var component = Qt.createComponent("Parser/DicomReaderEx.qml");
        var dicomReader = component.createObject(null, {
                                                   "viewer" : modelViewer
                                               });
        dicomReader.file = fileUrl;
    }

    function readReconstructor(fileUrls) {
        var component = Qt.createComponent("Parser/ReconstructorEx.qml");
        var reconstructor = component.createObject(null, {
                                                       "viewer" : modelViewer
                                                   });
        reconstructor.files = fileUrls;
    }

    function readSTL(fileUrl) {
        var component = Qt.createComponent("Parser/StlReaderEx.qml");
        var stlReader = component.createObject(null, {
                                                   "viewer" : modelViewer
                                               });
        stlReader.file = fileUrl;
    }

    FocusScope {
        focus: true;
        anchors.fill: parent;

        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_Escape: Qt.quit(); break;
            }
        }
    }

    Row {
        id: modelRow;

        Viewer.ModelViewerEx {
            id: modelViewer;
            width: appWindow.width - sidebar.width;
            height: appWindow.height - consoleDock.head.height;

            onPost: {
                switch (message.header.reciever) {
                    case "sidebar" :
                        sidebar.recieve(message);
                        break;
                    case "appWindow" :
                        appWindow.recieve(message);
                        break;
                    }
            }
        }
    }

    Dock.Sidebar {
        id: sidebar;

        anchors {
            left: modelRow.right;
            top: appWindow.top;
            bottom: appWindow.bottom;
            right: appWindow.right;
        }

        onPost: {
            if (message.header.reciever === "appWindow") {
                appWindow.recieve(message)
            }
            else {
                modelViewer.message = message;
            }
        }

        dX: appWindow.width * appWindow.sidebarWidth;
    }

    function toggleDocks() {
        sidebar.head.state = "expanded";
    }

    Dock.ConsoleDock {
        id: consoleDock;

        y: appWindow.height - head.height;

        width: modelViewer.width;

        anchors {
            left: parent.left;
        }

        dY: appWindow.height * appWindow.consoleDockHeight;
    }
}
