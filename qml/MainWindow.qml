import QtQuick 2.3;
import QtQuick.Controls 1.2;
import QtQuick.Dialogs 1.2;

import RenderTools 1.0;

import "qrc:/js/settings.js" as Settings;

import "Content" as Content;

ApplicationWindow {
    id: appWindow;
    width: 1100;
    height: 768;

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
                    viewerContent.settings.head.state = (viewerContent.settings.head.state === "collapsed") ? "expanded" : "collapsed";
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
                                                   "viewer" : viewerContent.viewer
                                               });
        dicomReader.file = fileUrl;
    }

    function readReconstructor(fileUrls) {
        var component = Qt.createComponent("Parser/ReconstructorEx.qml");
        var reconstructor = component.createObject(null, {
                                                       "viewer" : viewerContent.viewer
                                                   });
        reconstructor.files = fileUrls;
    }

    function readSTL(fileUrl) {
        var component = Qt.createComponent("Parser/StlReaderEx.qml");
        var stlReader = component.createObject(null, {
                                                   "viewer" : viewerContent.viewer
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

    Content.ViewerContent {
        id: viewerContent;

        width: parent.width;
        height: parent.height;

        onPost: {
            if (message.header.reciever === "appWindow") {
                appWindow.recieve(message);
            }
        }
    }
}
