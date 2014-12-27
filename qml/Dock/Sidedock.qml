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

    MouseArea {
        anchors {
            fill: parent;
        }

        hoverEnabled: true;

        onPositionChanged: {/*
            if (headingCanBeHidden) {
                switch (parent.state) {
                case "right" :
                    if (mouse.x <= heading.maximumHeight && heading.height) {
                        //heading.toggle();
                    }
                }
            }*/
        }
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

        property int parentX: 0;
        property int parentY: 0;

        transitions: [
            Transition {
                from: "collapsed";
                to: "expanded";

                ParallelAnimation {
                    ScriptAction {
                        script: sidedock.showContent = !sidedock.showContent;
                    }

                    NumberAnimation {
                        target: heading;
                        property: "x";
                        duration: animationSpeed;
                        to: sidedock.state === "left" ? sidedock.dX : sidedock.state === "right" ? - sidedock.dX : x;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: sidedock;
                        property: "x";
                        duration: animationSpeed;
                        to: sidedock.state === "left" ? sidedock.dX : sidedock.state === "right" ? - sidedock.dX : x;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: sidedock;
                        property: "width";
                        duration: animationSpeed;
                        to: sidedock.state === "left" || sidedock.state === "right" ? sidedock.width + sidedock.dX : width;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: heading;
                        property: "y";
                        duration: animationSpeed;
                        to: sidedock.state === "top" ? sidedock.dY : sidedock.state === "bottom" ? sidedock.dY : y;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: sidedock;
                        property: "y";
                        duration: animationSpeed;
                        to: sidedock.state === "top" ? 0 : sidedock.state === "bottom" ? parent.height - heading.height - sidedock.dY : y;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: sidedock;
                        property: "height";
                        duration: animationSpeed;
                        to: sidedock.state === "top" || sidedock.state === "bottom" ? sidedock.height + sidedock.dY : height;
                        easing.type: Easing.InOutQuad;
                    }
                }
            },

            Transition {
                from: "expanded";
                to: "collapsed";

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
                        to: sidedock.state === "left" || sidedock.state === "right" ?  sidedock.width - sidedock.dX : width;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: heading;
                        property: "y";
                        duration: animationSpeed;
                        to: sidedock.state === "top" || sidedock.state === "bottom" ? 0 : y;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: sidedock;
                        property: "y";
                        duration: animationSpeed;
                        to: sidedock.state === "top" ? 0 : sidedock.state === "bottom" ? parent.height - heading.height : y;
                        easing.type: Easing.InOutQuad;
                    }

                    NumberAnimation {
                        target: sidedock;
                        property: "height";
                        duration: animationSpeed;
                        to: sidedock.state === "top" ? heading.height : height;
                        easing.type: Easing.InOutQuad;
                    }
                }
            }
        ]
    }
}
