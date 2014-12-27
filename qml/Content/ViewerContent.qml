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

    signal post(var message);

    ModelViewerEx  {
        id: modelViewer;

        y: settingsDock.head.height;
        width: content.width - sidebar.width;
        height: content.height - consoleDock.head.height - settingsDock.head.height;

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

       anchors {
           left: parent.left;
       }

       dY: parent.height * parent.settingsDockHeight;
   }
}

