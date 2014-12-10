import QtQuick 2.3;

import RenderTools 1.0;

import "qrc:/js/helpers.js" as Helpers;

Viewport {
    id: viewport;

    property color color: "green";

    property bool invertedYAxis: false;

    property real minimumZoom: 0.2;
    property real maximumZoom: 10.0;

    property vector2d rotationSpeed: Qt.vector2d(width / 10, height / 10);

    property string currentPoint: "";

    signal rotate(var message);
    signal post(var message);

    signal setZoom(real zoomFactor, real x, real y);

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

        property real mouseXW: 0.0;
        property real mouseYW: 0.0;

        enabled: true;

        property bool rotating: false;

        hoverEnabled: true;

        anchors.fill: parent;

        onPressed: {
            switch (mouse.button) {
                case Qt.LeftButton:

                if (parent.currentPoint) {
                    parent.post({
                                    "header" : {
                                        "sender" : "viewport",
                                        "reciever" : "currentModel"
                                    },
                                    "data" : {
                                        "action" : "setPoint",
                                        "params" : {
                                            "id" : parent.currentPoint,
                                            "position" : Qt.vector3d(mouseX / width, mouseY / height, 0),
                                            "viewport" : parent
                                        }
                                    }
                                });
                }

                prevMouseX = mouseX;
                prevMouseY = mouseY;

                rotating = true;

                break;
            }
        }

        onPositionChanged: {
            if (rotating) {
                parent.rotate({
                                    "header" : {
                                        "sender" : "viewport",
                                        "reciever" : "currentModel"
                                    },
                                    "data" : {
                                        "action" : "rotate",
                                        "params" : {
                                            "angle" : Qt.vector3d(
                                                          (parent.invertedYAxis ? -1 : 1) * (prevMouseY - mouseY),
                                                          0.0,
                                                          mouseX - prevMouseX)
                                        }
                                    }
                                });

                prevMouseX = mouseX;
                prevMouseY = mouseY;
            }

            mouseXW = mouseX;
            mouseYW = mouseY;
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
                parent.setZoom(zoomFactor, mouseXW / parent.width, mouseYW / parent.height);
            }
        }
    }
}
