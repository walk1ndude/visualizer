import QtQuick 2.3

import "../js/pointsdictionary.js" as PointsDict
import "../js/helpers.js" as Helpers

Rectangle {
    id: individualInfo;
    width: 100;
    height: 300;

    color: "#cccccc";
    border.color: "black";
    border.width: 2;

    ListView {
        model: ListModel {
            id: listModel;

            Component.onCompleted: {
                var measures = PointsDict.pointsDict.measures;
                var groups = PointsDict.pointsDict.groups;
                var groupOrder = PointsDict.groupsOrder.IndividualInfo;

                for (var i = 0; i !== groupOrder.length; ++ i) {
                    var group = groups[groupOrder[i]];
                    append({
                               "itemName" : group,
                               "itemHeader" : group.text,
                               "itemText" : group.distances.text,
                               "subItems" : populateListElements(group.distances.point2point, measures)
                           });
                }

            }

            function populateListElements(ptps, measures) {
                var qmlElements = [];
                for (var ptp in ptps) {
                    var pointsOfPTP = ptps[ptp];

                    for (var i = 0; i !== pointsOfPTP.length; ++ i) {
                        var qmlElement = {};
                        qmlElement["from"] = measures[ptp].visuals.text;
                        qmlElement["to"] = measures[pointsOfPTP[i]].visuals.text;

                        qmlElements.push(qmlElement);
                    }
                }

                return qmlElements;
            }
        }

        delegate: delegateComponent;
        anchors.fill: parent;
    }

    Component {
        id: delegateComponent;

        Rectangle {
            id: mainRectangle;

            border {
                color: "black";
                width: 1;
            }

            color: "white";

            width: individualInfo.width;
            height: 60;

            Rectangle {
                id: borderRect;

                anchors {
                    fill: parent;
                    margins: 10;
                }

                border {
                    color: "black";
                    width: 3;
                }
            }

            Rectangle {
                id: header;
                color: "white";

                x: 20;
                y: 3;

                width: childrenRect.width;
                height: childrenRect.height;

                Text {
                    id: headerText;

                    font {
                        pixelSize: 16;
                        bold: true;
                    }

                    text: itemHeader;
                    wrapMode: Text.WordWrap;
                }
            }

            Loader {
                id: subItemLoader;

                anchors {
                    top: header.bottom;
                    left: header.left;
                }

                sourceComponent: subItemComponent;
                onStatusChanged: {
                    if (status == Loader.Ready) {
                        item.model = subItems;
                        item.headerText = itemText;

                        if (item.currentItem) {
                            mainRectangle.height = 60 + item.currentItem.height;
                        }
                    }
                }
            }
        }
    }

    Component {
        id: subItemComponent;

        GridView {
            id: gridView;

            property string headerText: "";

            header:
                Text {
                    id: headerText;
                    font {
                        pixelSize: 14;
                        italic: true;
                    }

                    text: gridView.headerText;
                    wrapMode: Text.WordWrap;
                }

            delegate: textMeasures;
        }
    }

    Component {
        id: textMeasures;
        Column {
            id: textMeasuresColumn;
            property variant model: GridView.view.model;
            Repeater {
                model: parent.model;
                delegate: Row {
                    Text {
                        id: from2To;
                        width: individualInfo.width - 40 - data.width;
                        text: from + " --> " + to;
                    }
                    Text {
                        id: data;
                        text: "Нет данных";
                    }
                }
            }
        }
    }
}
