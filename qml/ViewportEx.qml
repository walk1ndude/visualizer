import QtQuick 2.3

import RenderTools 1.0

import "../js/helpers.js" as Helpers

Viewport {
    id: viewport;

    property color color: "green";

    /* link to viewport array to which this
    one currently belongs */
    property variant array: parent;

    /* true - mouse events, captured in this
    also processed by other viewports in array
    marked by array property */
    property bool propagateToOthers: true;

    property bool invertedYAxis: false;

    property real minimumZoom: 0.2;
    property real maximumZoom: 10.0;

    property real rotationSpeed: 20.0;

    x: boundingRect.x * parent.width;
    y: boundingRect.y * parent.height;

    width: boundingRect.width * parent.width;
    height: boundingRect.height * parent.height;

    Rectangle {
        color: "transparent";

        anchors.fill: parent

        border {
            color: viewport.color;
        }

        Text {
            id: text;
            text: viewport.text;

            anchors {
                left: parent.left;
                top: parent.top;

                topMargin: 0.2 * text.height;
                leftMargin: 0.2 * text.height;
            }

            color: viewport.color;

            font {
                capitalization: Font.SmallCaps;
                pointSize: 24;
            }
        }
    }

    MouseArea {
        id: mouseArea;
        property real prevMouseX: 0.0;
        property real prevMouseY: 0.0;

        enabled: true;

        property bool rotating: false;

        hoverEnabled: true;

        anchors.fill: parent;

        onPressed: {
            switch (mouse.button) {
                case Qt.LeftButton:

                if (!rotating) {
                    parent.array.parent.addPoint(
                        Qt.point(
                            mouseX / width,
                            mouseY / height
                            ), parent
                        );
                }

                if (parent.projectionType == Viewport.PERSPECTIVE) {
                    prevMouseX = mouseX;
                    prevMouseY = mouseY;

                    rotating = !rotating;
                }

                break;
            }
        }

        onPositionChanged: {
            if (parent.projectionType == Viewport.PERSPECTIVE && rotating) {
                parent.array.parent.rotation = Qt.vector3d(
                            (parent.invertedYAxis ? 1 : -1) * (prevMouseY - mouseY) / parent.rotationSpeed,
                            (prevMouseX - mouseX) / parent.rotationSpeed,
                            0.0
                            );
            }
        }

        onReleased: {
            rotating = false;
        }

        onEntered: {
            parent.color = "red";
        }

        onExited: {
            prevMouseX = 0.0;
            prevMouseY = 0.0;

            parent.color = "green";
            rotating = false;
        }

        onWheel: {
            var zoomFactor = Helpers.clamp(parent.zoom + wheel.angleDelta.y * 0.001, parent.minimumZoom, parent.maximumZoom);

            if (zoomFactor !== parent.zoom) {
                if (parent.propagateToOthers) {
                    parent.array.zoom(zoomFactor);
                }
                else {
                    parent.zoom = zoomFactor;
                }

                parent.array.parent.update();
            }
        }
    }
}
