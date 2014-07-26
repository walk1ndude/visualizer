import QtQuick 2.3

import "../js/pointsdictionary.js" as PointsDict

Rectangle {
    id: measureGrid;
    width: 100;
    height: listModel.count * 45;

    color: "#cccccc"
    border {
        color: "black";
        width: 2;
    }

    property string selectedPointName: "";
    property color selectedPointColor: Qt.rgba(0, 0, 0, 0);

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
                               "itemName" : measureOrder[i],
                               "itemText" : measure.visuals.text,
                               "itemColor" : measure.visuals.color,
                               "selected" : false
                           });
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

                text: itemText;
                clip: true;
                wrapMode: Text.WordWrap;
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: if (mouse.button === Qt.LeftButton) {
                    listModel.get(index).selected = !selected;

                    if (listModel.prevIndex > -1) {
                        listModel.setProperty(listModel.prevIndex, "selected", false);
                    }

                    listModel.prevIndex = listModel.prevIndex === index ? -1 : index;

                    parent.color = selected ? itemColor : "white";

                    measureGrid.selectedPointName = itemName;
                    measureGrid.selectedPointColor = parent.color;
                }
            }
        }
    }
}
