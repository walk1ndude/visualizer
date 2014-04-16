import QtQuick 2.2

Item {
    id: hud

    signal rTopChanged(real value);
    signal rBottomChanged(real value);
    signal ambientIntensityChanged(real value);

    signal angleChanged(real xRot, real yRot, real zRot);
    signal zoomZChanged(real value);

    Rectangle {
        color: "#71888888"

        anchors.fill: parent

        ShaderGrid {
            anchors.left: parent.left
        }

        GeometryGrid {
            anchors.right: parent.right
        }
    }
}
