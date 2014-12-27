import QtQuick 2.3

import "../Dock" as Dock;

Rectangle {
    id: content;

    width: 100;
    height: 62;

    property real sidebarWidth: 0.35;
    property real consoleDockHeight: 0.4;
    property real settingsDockHeight: 0.4;

    property alias viewer: modelViewer;

    property alias settings: settingsDock;

    signal post(var message);

    ModelViewerEx  {
        id: modelViewer;

        width: content.width - sidebar.width;
        height: content.height - consoleDock.head.height;

        onPost: {
            switch (message.header.reciever) {
            case "sidebar" :
                sidebar.recieve(message);
                break;
            case "appWindow" :
                parent.post(message);
                break;
            }
        }

        function toggleDocks() {
            sidebar.head.state = "expanded";
        }
   }

   Dock.Sidebar {
       id: sidebar;

       anchors {
           top: parent.top;
           bottom: parent.bottom;
           right: parent.right;
       }

       onPost: {
           if (message.header.reciever === "appWindow") {
               parent.post(message);
           }
           else {
               modelViewer.message = message;
           }
       }

       dX: parent.width * parent.sidebarWidth;
   }

   Dock.ConsoleDock {
       id: consoleDock;

       y: parent.height - head.height;

       width: modelViewer.width;

       anchors {
           left: parent.left;
       }

       dY: parent.height * parent.consoleDockHeight;
   }

   Dock.SettingsDock {
       id: settingsDock;

       width: modelViewer.width;

       headingCanBeHidden: true;

       anchors {
           left: parent.left;
       }

       dY: parent.height * parent.settingsDockHeight;
   }

   MouseArea {
       anchors {
           top: modelViewer.top;
           bottom: modelViewer.bottom;
           right: modelViewer.right;
           bottomMargin: consoleDock.head.height;
           topMargin: settingsDock.head.height;
       }

       width: sidebar.head.state === "collapsed" ? 10 : 0;

       hoverEnabled: true;

       onEntered: {
           if (!sidebar.width) {
               sidebar.show();
           }
       }
   }

   Component.onCompleted: {
       sidebar.width = 0;
       settingsDock.height = 0;
   }
}

