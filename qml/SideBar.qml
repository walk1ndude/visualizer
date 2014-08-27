import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

import "../js/sidebarcontent.js" as SideBar
import "../js/helpers.js" as Helpers

Rectangle {
    id: sidebar;

    color: "#FFFFFF";

    property vector2d xRange: Qt.vector2d(0, 0);
    property vector2d yRange: Qt.vector2d(0, 0);
    property vector2d zRange: Qt.vector2d(0, 0);

    property int minHU: -1;
    property int maxHU: -1;

    property vector3d angle: Qt.vector3d(0, 0, 0);
    property real zoomFactor: 1.0;

    property string selectedPointName: "";
    property color selectedPointColor: Qt.rgba(0, 0, 0, 0);

    property int modelID: -1;

    signal updateIndividualInfo();
    signal distsUpdated();

    ListView {
        id: sidebarListView;
        model: ListModel {
            id: sidebarListModel;
            Component.onCompleted: {
                var sections = SideBar.sideBarDict.sections;
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

        anchors.fill: parent;
        delegate: sectionDelegate;
    }

    Component {
        id: sectionDelegate;
        Column {
            width: sidebar.width;

            Heading {
                width: sidebar.width;

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
                                     case "ShaderGrid" : return subSectionShaderGrid;
                                     case "GeometryGrid" : return subSectionGeometryGrid;
                                     case "MeasureGrid" : return subSectionMeasureGrid;
                                     case "IndividualInfo" : return subSectionIndividualInfo;
                                     default: return null;
                                     }
                                 }

                onStatusChanged: if (status == Loader.Ready) item.model = subSectionModel;
            }
        }
    }

    Component {
        id: subSectionShaderGrid;

        Column {
            property alias model : repeater.model;
            width: sidebar.width;
            Repeater {
                id: repeater;
                delegate:
                    ShaderGrid {
                    width: sidebar.width;

                    onXRangeChanged: sidebar.xRange = xRange;
                    onYRangeChanged: sidebar.yRange = yRange;
                    onZRangeChanged: sidebar.zRange = zRange;

                    //onMinHUChanged: sidebar.minHU = minHU;
                    //onMaxHUChanged: sidebar.maxHU = maxHU;
                }
            }
        }
    }

    Component {
        id: subSectionGeometryGrid;

        Column {
            property alias model : repeater.model;
            width: sidebar.width;
            Repeater {
                id: repeater;
                delegate:
                    GeometryGrid {
                    width: sidebar.width;

                    onAngleChanged: sidebar.angle = angle;
                    onZoomFactorChanged: sidebar.zoomFactor = zoomFactor;
                }
            }
        }
    }

    Component {
        id: subSectionMeasureGrid;

        Column {
            property alias model : repeater.model;
            width: sidebar.width;
            Repeater {
                id: repeater;
                delegate:
                    MeasureGrid {
                    id: measureGrid;
                    width: sidebar.width;

                    onSelectedPointNameChanged: sidebar.selectedPointName = selectedPointName;
                    onSelectedPointColorChanged: sidebar.selectedPointColor = selectedPointColor;
                }
            }
        }
    }

    Component {
        id: subSectionIndividualInfo;

        Column {
            property alias model : repeater.model;
            width: sidebar.width;
            Repeater {
                id: repeater;
                delegate:
                    IndividualInfo {
                        id: individualInfo
                        width: sidebar.width;

                        modelID: sidebar.modelID;

                        Connections {
                            target: sidebar
                            onUpdateIndividualInfo: individualInfo.updateIndividualInfo();
                        }

                        onDistsUpdated: sidebar.distsUpdated();
                }
            }
        }
    }
}

