import QtQuick 2.3
import QtQuick.Controls 1.2

Item {
    id: sidebar

    property color color: "#FFFFFF"

    signal xRangeChanged(vector2d xRange);
    signal yRangeChanged(vector2d yRange);
    signal zRangeChanged(vector2d zRange);

    signal minHUChanged(int minHU);
    signal maxHUChanged(int maxHU);

    signal angleChanged(vector3d angle);
    signal zoomFactorChanged(real zoomFactor);

    Rectangle {
        width: sidebar.width
        height: sidebar.height

        color: sidebar.color

        ListView {
            id: sidebarListView
            anchors.fill: parent
            model: sidebarListModel
            delegate: categoryDelegate
        }

        ListModel {
            id: sidebarListModel
            ListElement {
                categoryName: "Секущие плоскости"
                collapsed: true
                itemName: "shaderGrid"

                // A ListElement can't contain child elements, but it can contain
                // a list of elements. A list of ListElements can be used as a model
                // just like any other model type.
                subItems: ListElement { }
            }

            ListElement {
                categoryName: "Вращение"
                collapsed: true
                itemName: "geometryGrid"

                subItems: ListElement { }
            }

            ListElement {
                categoryName: "Линейки"
                collapsed: true
                itemName: "measureGrid"
                subItems: ListElement { }
            }

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
                        width: 30
                        height: 30
                        anchors.right: parent.right
                        anchors.rightMargin: 15
                        anchors.verticalCenter: parent.verticalCenter

                        onClicked: sidebarListModel.setProperty(index, "collapsed", !collapsed);
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
                                         switch(itemName) {
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

                                onXRangeChanged: sidebar.xRangeChanged(xRange);
                                onYRangeChanged: sidebar.yRangeChanged(yRange);
                                onZRangeChanged: sidebar.zRangeChanged(zRange);

                                //onMinHUChanged: sidebar.minHUChanged(minHU);
                                //onMaxHUChanged: sidebar.maxHUChanged(maxHU);
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

                                onAngleChanged: sidebar.angleChanged(angle);
                                onZoomFactorChanged: sidebar.zoomFactorChanged(zoomFactor);
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
                            }
                }
            }
        }
    }
}
