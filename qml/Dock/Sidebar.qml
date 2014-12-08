import QtQuick 2.3;

import "qrc:/js/sidebarcontent.js" as Sidebar;
import "qrc:/js/helpers.js" as Helpers;

import "Section" as Section;

Sidedock {
    id: sidebar;

    color: "#FFFFFF";

    heading: "Sidebar";

    state: "vertical";
    inverseFolding: false;

    signal distsUpdated();

    signal post(variant message);
    signal postToSections(variant message);

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
                               "collapsed" : sections[i].collapsed,
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

        visible: !!width;

        delegate: sectionDelegate;
    }

    Component {
        id: sectionDelegate;
        Column {
            width: sidebarListView.width;

            Heading {
                width: parent.width;

                text: sectionName;

                collapsed: collapsed;

                onCollapsedChanged: sidebarListModel.setProperty(index, "collapsed", !collapsed);
            }

            Loader {
                visible: !collapsed
                property variant subSectionModel : subSections;
                sourceComponent: if (!!collapsed) {
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

                onStatusChanged: if (status == Loader.Ready) item.model = subSectionModel;
            }
        }
    }

    Component {
        id: subSectionModelSpecs;

        Column {
            property alias model : repeater.model;
            width: sidebarListView.width;
            Repeater {
                id: repeater;
                delegate:
                    Section.ModelSpecs {
                        width: sidebarListView.width;

                        onPost: sidebar.post(message);
                }
            }
        }
    }

    Component {
        id: subSectionGeometry;

        Column {
            property alias model : repeater.model;
            width: sidebarListView.width;
            Repeater {
                id: repeater;
                delegate:
                    Section.Geometry {
                        width: sidebarListView.width;

                        onPost: sidebar.post(message);
                }
            }
        }
    }

    Component {
        id: subSectionMeasures;

        Column {
            property alias model : repeater.model;
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
                }
            }
        }
    }

    Component {
        id: subSectionIndividual;

        Column {
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

                        onDistsUpdated: sidebar.distsUpdated();
                }
            }
        }
    }
}

