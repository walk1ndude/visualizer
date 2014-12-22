import QtQuick 2.3;

import RenderTools 1.0;

import "qrc:/js/helpers.js" as Helpers;

Viewport {
    id: viewport;

    property color color: "green";

    property bool invertedYAxis: false;

    property real minimumZoom: 0.2;
    property real maximumZoom: 10.0;

    property int animationSpeed: 1000;

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

            visible: viewport.width;
        }

        Rectangle {
            color: "transparent";

            anchors {
                right: parent.right;
                top: parent.top;

                rightMargin: 0.4 * text.height;
                topMargin: 0.4 * text.height;
            }

            width: parent.width / 16;
            height: parent.width / 16;

            border {
                color: viewport.color;
            }

            Rectangle {
                anchors {
                    horizontalCenter: parent.horizontalCenter;
                    top: parent.top;
                    bottom: parent.bottom;
                    topMargin: parent.height / 8;
                    bottomMargin: parent.height / 8;
                }

                width: 1;
                height: parent.height * 3 / 4;

                color: viewport.color;

                visible: !viewport.isFullsized;
            }

            Rectangle {
                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: parent.left;
                    right: parent.right;
                    leftMargin: parent.width / 8;
                    rightMargin: parent.width / 8;
                }

                width: parent.width * 3 / 4;
                height: 1;

                color: viewport.color;
            }

            MouseArea {
                z: 2;

                anchors.fill: parent;

                enabled: true;

                cursorShape: Qt.PointingHandCursor;

                onClicked: {
                    switch (mouse.button) {
                        case Qt.LeftButton:
                            viewport.state = viewport.state === "normal" ? "fullsized" : "normal";
                            break;
                    }
                }
            }
        }
    }

    state: "normal";

    states: [
        State {
            name: "normal";
        },
        State {
            name: "fullsized"
        }
    ]

    property rect prevRect: Qt.rect(0.0, 0.0, 0.0, 0.0);

    transitions: [
        Transition {
            from: "normal";
            to: "fullsized";
            SequentialAnimation {
                ParallelAnimation {
                    NumberAnimation {
                        target: viewport;
                        property: "x";
                        duration: animationSpeed;
                        from: x;
                        to: 0;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: viewport;
                        property: "y";
                        duration: animationSpeed;
                        from: y;
                        to: 0;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: viewport;
                        property: "width";
                        duration: animationSpeed;
                        from: width;
                        to: parent.width;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: viewport;
                        property: "height";
                        duration: animationSpeed;
                        from: height;
                        to: parent.height;
                        easing.type: Easing.InOutQuad;
                    }
                }
                ScriptAction {
                    script: {
                        prevRect = boundingRect;
                        parent.hideExcept(viewport);
                    }
                }
            }
        },

        Transition {
            from: "fullsized";
            to: "normal";
            SequentialAnimation {
                ScriptAction {
                    script: parent.restore();
                }

                ParallelAnimation {
                    NumberAnimation {
                        target: viewport;
                        properties: "x";
                        duration: animationSpeed;
                        from: 0;
                        to: prevRect.x * parent.width;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: viewport;
                        property: "y";
                        duration: animationSpeed;
                        from: 0;
                        to: prevRect.y * parent.height;
                        easing.type: Easing.InOutQuad;
                    }


                    NumberAnimation {
                        target: viewport;
                        property: "width";
                        duration: animationSpeed;
                        from: parent.width;
                        to: prevRect.width * parent.width;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: viewport;
                        property: "height";
                        duration: animationSpeed;
                        from: parent.height;
                        to: prevRect.height * parent.height;
                        easing.type: Easing.InOutQuad;
                    }
                }
            }
        }
    ]

    MouseArea {
        id: mouseArea;

        property real prevMouseX: 0.0;
        property real prevMouseY: 0.0;

        property real mouseXW: 0.0;
        property real mouseYW: 0.0;

        enabled: true;

        property bool rotating: false;

        hoverEnabled: true;

        propagateComposedEvents: true;

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

        onClicked: {
            mouse.accepted = false;
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
