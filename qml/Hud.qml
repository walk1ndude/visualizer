import QtQuick 2.2

Item {
    id: hud

    signal rTopChanged(real value);
    signal rBottomChanged(real value);
    signal ambientIntensityChanged(real value);

    signal angleChanged(real xRot, real yRot, real zRot);
    signal zoomZChanged(real value);

    visible: false

    Rectangle {
        id: hudRectangle
        color: "#71888888"
        width: 640
        height: 100
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

    function show() {
        hud.visible = true;
    }
}
