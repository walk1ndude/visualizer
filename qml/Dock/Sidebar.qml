import QtQuick 2.3;

import "qrc:/js/sidebarcontent.js" as Sidebar;
import "qrc:/js/helpers.js" as Helpers;

import "Section" as Section;

Sidedock {
    id: sidebar;

    color: "#FFFFFF";

    heading: "Sidebar";

    state: "right";

    signal post(var message);
    signal postToSections(var message);

    function recieve(message) {
        postToSections(message);
    }

    ListView {
        id: sidebarListView;
        model: ListModel {
            id: sidebarListModel;
            Component.onCompleted: {
                var sections = Sidebar.sidebarDict.sections;
                for (var i = 0; i !== sections.length; ++ i) {
                    append({
                               "sectionName" : sections[i].text,
                               "_state" : sections[i].state,
                               "sectionType" : sections[i].type,
                               "subSections" : [ { } ]
                           });
                }
            }
        }

        anchors {
            right: parent.right;
            top: parent.top;
            bottom: parent.bottom;
        }

        width: parent.width - parent.head.height;

        visible: showContent;

        delegate: sectionDelegate;
    }

    Component {
        id: sectionDelegate;
        Column {
            id: sectionColumn;
            width: sidebarListView.width;

            property int sumHeight: 0;

            Heading {
                id: heading;

                width: parent.width;

                text: sectionName;

                state: _state;

                onStateChanged: sidebarListModel.setProperty(index, "_state", state === "collapsed" ? "collapsed" : "expanded");

                transitions: [
                    Transition {
                        from: "expanded";
                        to: "collapsed";

                        ParallelAnimation {
                            NumberAnimation {
                                target: sectionColumn;
                                property: "height";
                                duration: sidebar.animationSpeed;
                                to: heading.height;
                                easing.type: Easing.InOutQuad;
                            }
                        }
                    },

                    Transition {
                        from: "collapsed";
                        to: "expanded";

                        ParallelAnimation {
                            NumberAnimation {
                                target: sectionColumn;
                                property: "height";
                                duration: sidebar.animationSpeed;
                                to: sectionColumn.sumHeight;
                                easing.type: Easing.InOutQuad;
                            }
                        }
                    }
                ]
            }

            Loader {
                id: sectionLoader;

                property var subSectionModel : subSections;

                sourceComponent: {
                                if (_state === "collapsed") {
                                     return null;
                                 }
                                 else {
                                     switch(sectionType) {
                                     case "ModelSpecs" : return subSectionModelSpecs;
                                     case "Geometry" : return subSectionGeometry;
                                     case "Measures" : return subSectionMeasures;
                                     case "Individual" : return subSectionIndividual;
                                     default: return null;
                                     }
                                 }
                }

                onStatusChanged: if (status == Loader.Ready) {
                                     item.model = subSectionModel;
                                     sectionColumn.sumHeight = heading.height + item.height;
                                 }
            }
        }
    }

    Component {
        id: subSectionModelSpecs;

        Column {
            id: column;

            property alias model : repeater.model;
            width: sidebarListView.width;

            Repeater {
                id: repeater;
                delegate:
                    Section.ModelSpecs {
                        width: sidebarListView.width;

                        onPost: sidebar.post(message);

                        Connections {
                            target: sidebar;
                            onPostToSections : recieve(message);
                        }

                        onHeightChanged: column.height = height;
                }
            }
        }
    }

    Component {
        id: subSectionGeometry;

        Column {
            id: column;

            property alias model : repeater.model;
            width: sidebarListView.width;
            Repeater {
                id: repeater;
                delegate:
                    Section.Geometry {
                        width: sidebarListView.width;

                        onPost: sidebar.post(message);

                        onHeightChanged: column.height = height;
                }
            }
        }
    }

    Component {
        id: subSectionMeasures;

        Column {
            id: column;

            property alias model: repeater.model;

            width: sidebarListView.width;
            Repeater {
                id: repeater;
                delegate:
                    Section.Measures {
                        id: measures;
                        width: sidebarListView.width;

                        onPost: sidebar.post(message);

                        Connections {
                            target: sidebar;
                            onPostToSections : recieve(message);
                        }

                        onHeightChanged: column.height = height;
                }
            }
        }
    }

    Component {
        id: subSectionIndividual;

        Column {
            id: column;

            property alias model : repeater.model;
            width: sidebarListView.width;
            Repeater {
                id: repeater;
                delegate:
                    Section.Individual {
                        id: individual
                        width: sidebarListView.width;

                        Connections {
                            target: sidebar;
                            onPostToSections: recieve(message);
                        }

                        onPost: sidebar.post(message);

                        onHeightChanged: column.height = height;
                }
            }
        }
    }
}

