import QtQuick 2.3;

Rectangle {
    id: sidedock;
    height: heading.height;

    property string heading: "";
    property var head: heading;

    property bool showContent: false;

    property int animationSpeed: 1000;

    property alias headingCanBeHidden: heading.canBeHidden;

    property real dX: 300;
    property real dY: 300;

    clip: true;

    states: [
        State {
            name: "top";

            AnchorChanges {
                target: heading;

                anchors {
                    left: parent.left;
                    right: parent.right;
                }
            }

            PropertyChanges {
                target: sidedock;
                height: heading.height;
            }
        },

        State {
            name: "bottom";

            AnchorChanges {
                target: heading;

                anchors {
                    left: parent.left;
                    right: parent.right;
                }
            }

            PropertyChanges {
                target: sidedock;
                height: heading.height;
            }
        },

        State {
            name: "left";

            PropertyChanges {
                target: heading;
                width: parent.height;
                transform: [
                    translateTo,
                    rotate,
                    translateBack
                ]
            }

            PropertyChanges {
                target: sidedock;
                width: heading.height;
                height: parent ? parent.height : parent.dX;
            }
        },

        State {
            name: "right";

            PropertyChanges {
                target: heading;
                width: parent.height;
                transform: [
                    translateTo,
                    rotate,
                    translateBack
                ]
            }

            PropertyChanges {
                target: sidedock;
                width: heading.height;
                height: parent ? parent.height : parent.dX;
            }
        }
    ]

    // for saving proportions of docks on window resizing
    onDXChanged: {
        if ((state === "left" || state === "right") && heading.state === "expanded") {
            var oldW = width;
            // height 'cause of transformations
            width = dX + heading.height;
            heading.x += (oldW - width)
        }
    }

    onDYChanged: {
        if (heading.state === "expanded") {
            if (state === "bottom") {
                heading.y = sidedock.dY;
                y = parent.height - heading.height - dY;
                height = height + dY;
            }
            else if (state === "top") {
                heading.y = sidedock.dY;
                y = 0;
                height = heading.height + dY;
            }
        }
    }

    state: "right";


    function show() {
        width = heading.height;
    }

    function hide() {
        width = 0;
    }

    // transformations to make heading vertical
    Translate {
        id: translateTo;
        x: - width / 2;
        y: - height / 2;
    }

    Rotation {
        id: rotate;
        angle: 90;

        axis {
            x: 0;
            y: 0;
            z: 1;
        }
    }

    Translate {
        id: translateBack;
        x: height / 2 + width - height;
        y: width / 2;
    }

    color: "#AAFFFFFF";

    Heading {
        id: heading;
        text: parent.heading;

        transitions: [
            Transition {
                from: "collapsed";
                to: "expanded";

                SequentialAnimation {
                    ScriptAction {
                        script: {
                            var horizontal = state === "left" || state === "right";
                            width = horizontal ? heading.height : heading.width;
                        }
                    }

                    ParallelAnimation {
                        ScriptAction {
                            script: sidedock.showContent = !sidedock.showContent;
                        }

                        NumberAnimation {
                            target: heading;
                            property: "x";
                            duration: animationSpeed;
                            to: state === "left" ? dX : state === "right" ? - dX : x;
                            easing.type: Easing.InOutQuad;
                        }

                        NumberAnimation {
                            target: sidedock;
                            property: "x";
                            duration: animationSpeed;
                            to: state === "left" ? dX : state === "right" ? - dX : x;
                            easing.type: Easing.InOutQuad;
                        }

                        NumberAnimation {
                            target: sidedock;
                            property: "width";
                            duration: animationSpeed;
                            to: state === "left" || state === "right" ? heading.maximumHeight + dX : width;
                            easing.type: Easing.InOutQuad;
                        }

                        NumberAnimation {
                            target: heading;
                            property: "y";
                            duration: animationSpeed;
                            to: state === "top" ? dY - heading.height : state === "bottom" ? dY : y;
                            easing.type: Easing.InOutQuad;
                        }

                        NumberAnimation {
                            target: sidedock;
                            property: "y";
                            duration: animationSpeed;
                            to: state === "top" ? 0 : state === "bottom" ? parent.height - heading.height - dY : y;
                            easing.type: Easing.InOutQuad;
                        }

                        NumberAnimation {
                            target: sidedock;
                            property: "height";
                            duration: animationSpeed;
                            to: state === "top" ? dY : state === "bottom" ? heading.height + dY : height;
                            easing.type: Easing.InOutQuad;
                        }
                    }
                }
            },

            Transition {
                from: "expanded";
                to: "collapsed";

                SequentialAnimation {
                    ParallelAnimation {
                        ScriptAction {
                            script: sidedock.showContent = !sidedock.showContent;
                        }

                        NumberAnimation {
                            target: heading;
                            property: "x";
                            duration: animationSpeed;
                            to: 0
                            easing.type: Easing.InOutQuad;
                        }

                        NumberAnimation {
                            target: sidedock;
                            property: "x";
                            duration: animationSpeed;
                            to: 0;
                            easing.type: Easing.InOutQuad;
                        }

                        NumberAnimation {
                            target: sidedock;
                            property: "width";
                            duration: animationSpeed;
                            to: state === "left" || state === "right" ? width - dX : width;
                            easing.type: Easing.InOutQuad;
                        }

                        NumberAnimation {
                            target: heading;
                            property: "y";
                            duration: animationSpeed;
                            to: state === "top" || state === "bottom" ? 0 : y;
                            easing.type: Easing.InOutQuad;
                        }

                        NumberAnimation {
                            target: sidedock;
                            property: "y";
                            duration: animationSpeed;
                            to: state === "top" ? 0 : state === "bottom" ? parent.height - heading.height : y;
                            easing.type: Easing.InOutQuad;
                        }

                        NumberAnimation {
                            target: sidedock;
                            property: "height";
                            duration: animationSpeed;
                            to: state === "top" ? heading.height : height;
                            easing.type: Easing.InOutQuad;
                        }
                    }


                    ParallelAnimation {
                        NumberAnimation {
                            target: sidedock;
                            property: "width";
                            duration: animationSpeed / 2;
                            to: heading.canBeHidden && (state === "left" || state === "right") ? 0 : width;
                            easing.type: Easing.InOutQuad;
                        }

                        NumberAnimation {
                            target: sidedock;
                            property: "height";
                            duration: animationSpeed / 2;
                            to: heading.canBeHidden && (state === "top" || state === "bottom") ? 0 : height;
                            easing.type: Easing.InOutQuad;
                        }
                    }
                }
            }
        ]
    }
}
