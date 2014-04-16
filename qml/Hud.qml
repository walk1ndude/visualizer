import QtQuick 2.2

Item {
    id: hud

    signal rTopChanged(real value);
    signal rBottomChanged(real value);
    signal ambientIntensityChanged(real value);

    signal angleChanged(real xRot, real yRot, real zRot);
    signal zoomZChanged(real value);

    Rectangle {
        id: hudRectangle
        color: "#71888888"

        anchors.fill: parent

        Row {
            y: 10
            spacing: hudRectangle.width * 0.05

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
}
