import QtQuick 2.3

import "../js/pointsdictionary.js" as PointsDict

Rectangle {
    id: measureGrid
    width: 100
    height: 300

    color: "#cccccc"
    border.color: "black"
    border.width: 2

    property string index: ""

    ListView {
        model: ListModel {
            id: listModel

            property int prevIndex: -1

            Component.onCompleted: {
                for (var key in PointsDict.pointsDict) {
                    append({
                               "itemName" : key,
                               "itemText" : PointsDict.pointsDict[key].text,
                               "itemColor" : PointsDict.pointsDict[key].color,
                               "selected" : false
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
            id: rectangleItem
            border.color: "black"
            border.width: 1
            color: "white"

            property alias textItem: textItem

            height: 45
            width: measureGrid.width

            Text {
                id: textItem
                anchors.verticalCenter: parent.verticalCenter
                x: 15
                font.pixelSize: 12
                font.bold: selected
                text: itemText
                clip: true
                wrapMode: Text.WordWrap
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    listModel.get(index).selected = !selected;

                    if (listModel.prevIndex > -1) {
                        listModel.setProperty(listModel.prevIndex, "selected", false);
                    }

                    listModel.prevIndex = listModel.prevIndex === index ? -1 : index;

                    parent.color = selected ? itemColor : "white";
                    measureGrid.index = itemName;
                }
            }
        }
    }
}
