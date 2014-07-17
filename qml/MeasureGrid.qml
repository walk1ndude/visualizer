import QtQuick 2.3

import "../js/pointsdictionary.js" as PointsDict

Rectangle {
    id: measureGrid
    width: 100
    height: 300

    color: "#cccccc"
    border.color: "black"
    border.width: 2

    ListView {
        model: ListModel {
            id: listModel
            Component.onCompleted: {
                for (var key in PointsDict.pointsDict) {
                    append({
                               "itemName" : key,
                               "colorItem" : PointsDict.pointsDict[key],
                           });
                }
            }
        }

        delegate: delegateComponent
        anchors.fill: parent
    }

    Component {
        id: delegateComponent

            Rectangle {
                id: categoryItem
                border.color: "black"
                border.width: 1
                color: "white"

                property bool selected: false

                height: 45
                width: measureGrid.width

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    x: 15
                    font.pixelSize: 12
                    text: itemName
                    clip: true
                    wrapMode: Text.WordWrap
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        parent.selected = !parent.selected;
                        parent.color = categoryItem.selected ? listModel.get(index).colorItem : "white";
                    }
                }
            }
    }
}
