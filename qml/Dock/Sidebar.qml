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

    property variant selectedPoint: ({});

    signal togglePoint(string point);

    property int modelID: -1;

    signal updateIndividual();
    signal distsUpdated();

    signal post(variant message);

    function updatePoint(point) {
        measuredPoint = point.name;
        measuredPosition = point.position;
    }

    property string measuredPoint: "";
    property vector3d measuredPosition: Qt.vector3d(0, 0, 0);

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

                        onAngleChanged: sidebar.angle = angle;
                        onZoomFactorChanged: sidebar.zoomFactor = zoomFactor;
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
                        width: sidebarListView.width;

                        onSelectedPointChanged: sidebar.selectedPoint = selectedPoint;
                        onTogglePoint: sidebar.togglePoint(point);

                        Connections {
                            target: sidebar;
                            onMeasuredPositionChanged: {
                                updatePoint({
                                                "name" : measuredPoint,
                                                "position" : measuredPosition
                                            });
                            }
                        }

                        Connections {
                            target: sidebar;
                            onMeasuredPositionChanged: {
                                updatePoint({
                                                "name" : measuredPoint,
                                                "position" : measuredPosition
                                            });
                            }
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

                        modelID: sidebar.modelID;

                        Connections {
                            target: sidebar
                            onUpdateIndividual: individual.updateIndividual();
                        }

                        onDistsUpdated: sidebar.distsUpdated();
                }
            }
        }
    }
}

