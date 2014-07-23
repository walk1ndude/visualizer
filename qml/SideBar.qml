import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

import "../js/sidebarcontent.js" as SideBar

Item {
    id: sidebar

    property color color: "#FFFFFF"

    property vector2d xRange: Qt.vector2d(0, 0)
    property vector2d yRange: Qt.vector2d(0, 0)
    property vector2d zRange: Qt.vector2d(0, 0)

    property int minHU: -1
    property int maxHU: -1

    property vector3d angle: Qt.vector3d(0, 0, 0)
    property real zoomFactor: 1.0

    property string selectedPointName: ""
    property color selectedPointColor: Qt.rgba(0, 0, 0, 0)

    Rectangle {
        width: sidebar.width
        height: sidebar.height

        color: sidebar.color

        ListView {
            id: sidebarListView
            model: ListModel {
                id: sidebarListModel
                Component.onCompleted: {
                    var elements = SideBar.sideBarDict.elements;
                    for (var i = 0; i !== elements.length; ++ i) {
                        append({
                                   "categoryName" : elements[i].text,
                                   "collapsed" : true,
                                   "itemType" : elements[i].type,
                                   "subItems" : Qt.createQmlObject("import QtQuick 2.3; ListElement { }", categoryDelegate)
                               });
                    }
                }
            }

            anchors.fill: parent
            delegate: categoryDelegate
        }

        Component {
            id: categoryDelegate
            Column {
                width: sidebar.width

                Rectangle {
                    id: categoryItem
                    border.color: "black"
                    border.width: 5
                    color: "white"

                    height: 50
                    width: sidebar.width

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        x: 15
                        font.pixelSize: 14
                        text: categoryName
                    }

                    Button {
                        width: 20
                        height: 20
                        anchors.right: parent.right
                        anchors.rightMargin: 15
                        anchors.topMargin: 5
                        anchors.verticalCenter: parent.verticalCenter

                        iconSource: "qrc:/icons/expand.svg"

                        style: ButtonStyle {
                            background: Rectangle {
                                anchors.fill: parent
                            }
                        }

                        onClicked: {
                            sidebarListModel.setProperty(index, "collapsed", !collapsed);

                            iconSource = sidebarListModel.get(index).collapsed ? "qrc:/icons/expand.svg" : "qrc:/icons/collapse.svg";
                        }
                    }
                }

                Loader {
                    id: subItemLoader

                    // This is a workaround for a bug/feature in the Loader element. If sourceComponent is set to null
                    // the Loader element retains the same height it had when sourceComponent was set. Setting visible
                    // to false makes the parent Column treat it as if it's height was 0.
                    visible: !collapsed
                    property variant subItemModel : subItems
                    sourceComponent: if (!!collapsed) {
                                         return null;
                                     }
                                     else {
                                         switch(itemType) {
                                         case "shaderGrid": return subItemShaderGrid
                                         case "geometryGrid": return subItemGeometryGrid
                                         case "measureGrid": return subItemMeasureGrid
                                         default: return null;
                                         }
                                     }

                    onStatusChanged: if (status == Loader.Ready) item.model = subItemModel
                }
            }
        }

        Component {
            id: subItemShaderGrid

            Column {
                property alias model : subItemRepeaterShaderGrid.model
                width: sidebar.width
                Repeater {
                    id: subItemRepeaterShaderGrid
                    delegate:
                            ShaderGrid {
                                width: sidebar.width

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
            id: subItemGeometryGrid

            Column {
                property alias model : subItemRepeaterGeometryGrid.model
                width: sidebar.width
                Repeater {
                    id: subItemRepeaterGeometryGrid
                    delegate:
                            GeometryGrid {
                                width: sidebar.width

                                onAngleChanged: sidebar.angle = angle;
                                onZoomFactorChanged: sidebar.zoomFactor = zoomFactor;
                            }
                }
            }
        }

        Component {
            id: subItemMeasureGrid

            Column {
                property alias model : subItemRepeaterMeasureGrid.model
                width: sidebar.width
                Repeater {
                    id: subItemRepeaterMeasureGrid
                    delegate:
                            MeasureGrid {
                                width: sidebar.width

                                onSelectedPointNameChanged: sidebar.selectedPointName = selectedPointName;
                                onSelectedPointColorChanged: sidebar.selectedPointColor = selectedPointColor;
                            }
                }
            }
        }
    }
}
