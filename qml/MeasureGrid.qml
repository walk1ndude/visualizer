import QtQuick 2.3

Rectangle {
    id: measureGrid
    width: 100
    height: 300

    color: "#cccccc"
    border.color: "black"
    border.width: 2

    ListView {
        model: listModel
        delegate: delegateComponent
        anchors.fill: parent
    }

    ListModel {
        id: listModel
        ListElement { itemName: "Верхняя резцовая" }
        ListElement { itemName: "Правый сустав" }
        ListElement { itemName: "Левый сустав" }
        ListElement { itemName: "Правая опорная точка протетической плоскости" }
        ListElement { itemName: "Левая опорная точка протетической плоскости" }
    }

    Component {
        id: delegateComponent

            Rectangle {
                id: categoryItem
                border.color: "black"
                border.width: 1
                color: "white"

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
            }

    }
}
