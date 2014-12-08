import QtQuick 2.3;

import "qrc:/js/pointsdictionary.js" as PointsDict
import "qrc:/js/helpers.js" as Helpers

Rectangle {
    id: measures;
    width: 100;
    height: listModel.count * 45;

    color: "#cccccc"
    border {
        color: "black";
        width: 2;
    }

    signal post(variant message);

    ListView {
        id: listView;
        model: ListModel {
            id: listModel;

            Component.onCompleted: {
                var measuresDict = PointsDict.pointsDict.measures;
                var measureOrder = PointsDict.measuresOrder["Measures"];

                for (var i = 0; i !== measureOrder.length; ++ i) {
                    var measure = measuresDict[measureOrder[i]];
                    append({
                               "itemId" : measureOrder[i],
                               "itemName" : measure.name,
                               "itemColor" : measure.color,
                               "shown" : false,
                               "position" : ""
                           });

                    Helpers.addInPointToGroup(measureOrder[i], PointsDict.pointsDict.groups);
                }

                listView.currentIndex = -1;
            }
        }

        delegate: delegateComponent;
        anchors.fill: parent;

        onCurrentIndexChanged: {
            /*
            if (currentIndex === -1) {
                measures.selectedPoint = { };
            }*/
        }
    }

    function updatePoint(point) {
        for (var i = 0; i !== listModel.count; ++ i) {
            if (listModel.get(i).itemId === point.name) {
                listModel.setProperty(i, "position", " [" + point.position.x.toFixed(2) + ", "
                                      + point.position.y.toFixed(2) + ", "
                                      + point.position.z.toFixed(2) + "]");
            }
        }
    }

    Component {
        id: delegateComponent;

        Rectangle {
            property bool isShown: shown;

            border {
                color: "black";
                width: 1;
            }

            color: shown ? itemColor : "white";

            height: 45;
            width: measures.width;

            Text {
                anchors.verticalCenter: parent.verticalCenter;
                x: 15;

                font {
                    pixelSize: 12;
                    bold: listView.currentIndex === index;
                }

                text: itemName + position;
                clip: true;
                wrapMode: Text.WordWrap;
            }

            onIsShownChanged: measures.post({
                              "header" : {
                                  "sender" : "measures",
                                  "reciever" : "currentModel"
                              },
                              "data" : {
                                  "action" : "togglePoint",
                                  "params" : {
                                      "name" : itemId
                                  }
                              }
                          });

            MouseArea {
                anchors {
                    fill : parent
                }

                acceptedButtons: Qt.LeftButton | Qt.RightButton;

                onClicked: {
                    if (modelID === "") {
                        return;
                    }

                    if (mouse.button === Qt.LeftButton) {
                        listView.currentIndex = (listView.currentIndex === index) ? -1 : index;

                        if (listView.currentIndex !== -1) {
                            listModel.setProperty(listView.currentIndex, "shown", true);

                            measures.post({
                                              "header" : {
                                                  "sender" : "measures",
                                                  "reciever" : "currentModel"
                                              },
                                              "data" : {
                                                  "action" : "addPoint",
                                                  "params" : {
                                                      "name" : itemId,
                                                      "color" : parent.color,
                                                      "groups" : Helpers.pointInGroups[itemId]
                                                  }
                                              }
                                          });
                        }
                    }
                    else if (mouse.button === Qt.RightButton) {
                        listModel.setProperty(index, "shown", false);

                        if (listView.currentIndex === index) {
                            listView.currentIndex = -1;
                        }
                    }
                }
            }
        }
    }
}
