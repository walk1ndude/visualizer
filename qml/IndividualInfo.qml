import QtQuick 2.3

import "../js/pointsdictionary.js" as PointsDict
import "../js/helpers.js" as Helpers

Rectangle {
    id: invidualInfo
    width: 100
    height: 300

    color: "#cccccc"
    border.color: "black"
    border.width: 2

    ListView {
        model: ListModel {
            id: listModel

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
                               "subItems" : populateListElements(group.distances.point2point, measures, subItemComponent)
                           });
                }

            }

            function populateListElements(ptps, measures, parent) {
                var qmlElements = [];
                for (var ptp in ptps) {
                    var pointsOfPTP = ptps[ptp];

                    for (var i = 0; i !== pointsOfPTP.length; ++ i) {
                        qmlElements.push(
                                    Qt.createQmlObject(Helpers
                                    .insertElementsIndivGroupTemplate
                                    .replace("{fromP}", "\"" + measures[ptp].visuals.text + "\"")
                                    .replace("{toP}", "\"" + measures[pointsOfPTP[i]].visuals.text + "\""), parent));
                    }
                }

                return qmlElements;
            }
        }

        delegate: delegateComponent
        anchors.fill: parent
    }

    Component {
        id: delegateComponent

        Rectangle {
            id: mainRectangle

            border {
                color: "black"
                width: 1
            }

            color: "white"

            width: invidualInfo.width
            height: 100

            Rectangle {
                anchors {
                    fill: parent
                    margins: 10
                }

                border {
                    color: "black"
                    width: 3
                }
            }

            Rectangle {
                id: header
                color: "white"

                x: 20
                y: 3

                width: childrenRect.width
                height: childrenRect.height

                Text {
                    font {
                        pixelSize: 16
                        bold: true
                    }

                    text: itemHeader
                    wrapMode: Text.WordWrap
                }
            }

            Text {
                id: sourceText
                anchors {
                    top: header.bottom
                    left: header.left
                }

                font {
                    pixelSize: 14
                    italic: true
                }

                text: itemText
                wrapMode: Text.WordWrap
            }

            Rectangle {
                id: helper

                anchors {
                    top: sourceText.bottom
                    left: sourceText.left
                }

                Loader {
                    id: subItemLoader

                    property variant subItemModel: subItems
                    sourceComponent: subItemComponent
                    onStatusChanged: if (status == Loader.Ready) {
                                         item.model = subItems;
                                         mainRectangle.height = item.model.count * 50;
                                     }
                }
            }
        }
    }

    Component {
        id: subItemComponent

        Column {
            property alias model: subItemComponentRepeater.model
            width: invidualInfo.width
            Repeater {
                id: subItemComponentRepeater
                delegate: textMeasures
            }
        }
    }

    Component {
        id: textMeasures
        Row {
            width: invidualInfo.width
            spacing: 5
            Text {
                text: "От " + from + " до " + to
            }
            Text {
                text: "Нет данных"
            }
        }
    }
}
