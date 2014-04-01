import QtQuick 2.2

Rectangle {
    id: hud
    color: "#71888888"
    width: 640
    height: 200
    focus: true

    Row {
        y: 10
        spacing: 50

        ShaderGrid {
            id: shaderGrid
            objectName: "shaderGrid"
        }

        GeometryGrid {
            id: geometryGrid
            objectName: "geometryGrid"
        }
    }
}
