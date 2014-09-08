import QtQuick 2.3

import "../js/pointsdictionary.js" as PointsDict
import "../js/helpers.js" as Helpers

Rectangle {
    id: measureGrid;
    width: 100;
    height: listModel.count * 45;

    color: "#cccccc"
    border {
        color: "black";
        width: 2;
    }

    property variant selectedPoint: ({});

    ListView {
        model: ListModel {
            id: listModel;

            property int prevIndex: -1;

            Component.onCompleted: {
                var measures = PointsDict.pointsDict.measures;
                var measureOrder = PointsDict.measuresOrder.MeasuresGrid;

                for (var i = 0; i !== measureOrder.length; ++ i) {
                    var measure = measures[measureOrder[i]];
                    append({
                               "itemId" : measureOrder[i],
                               "itemName" : measure.name,
                               "itemColor" : measure.color,
                               "selected" : false,
                               "shown" : false
                           });

                    Helpers.addInPointToGroup(measureOrder[i], PointsDict.pointsDict.groups);
                }
            }
        }

        delegate: delegateComponent;
        anchors.fill: parent;
    }

    Component {
        id: delegateComponent;

        Rectangle {
            border {
                color: "black";
                width: 1;
            }
            color: "white";

            height: 45;
            width: measureGrid.width;

            Text {
                anchors.verticalCenter: parent.verticalCenter;
                x: 15;

                font {
                    pixelSize: 12;
                    bold: selected;
                }

                text: itemName;
                clip: true;
                wrapMode: Text.WordWrap;
            }

            MouseArea {
                anchors {
                    fill : parent
                }

                onClicked: {
                    if (mouse.button === Qt.LeftButton) {
                        var currentElement = listModel.get(index);

                        if (currentElement.shown) {
                            currentElement.selected = false;
                            currentElement.shown = false;
                        }
                        else {
                            currentElement.selected = !selected;
                        }

                        if (listModel.prevIndex > -1) {
                            listModel.setProperty(listModel.prevIndex, "selected", false);
                        }

                        var clickedOnSame = listModel.prevIndex === index;

                        listModel.prevIndex = clickedOnSame ? -1 : index;

                        parent.color = shown ? itemColor : "white";

                        measureGrid.selectedPoint = (listModel.prevIndex === -1) ? { } : {
                                    "name" : itemId,
                                    "color" : parent.color,
                                    "groups" : Helpers.pointInGroups[itemId]
                        };
                    }
                }
            }
        }
    }
}
