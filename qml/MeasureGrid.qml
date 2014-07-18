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
                var measures = PointsDict.pointsDict.measures;
                for (var i = 0; i !== measures.length; ++ i) {
                    append({
                               "itemName" : measures[i].name,
                               "itemText" : measures[i].visuals.text,
                               "itemColor" : measures[i].visuals.color,
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
            border.color: "black"
            border.width: 1
            color: "white"

            height: 45
            width: measureGrid.width

            Text {
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
