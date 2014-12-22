import QtQuick 2.3;

import "qrc:/qml/Control" as Control;

Rectangle {
    id: modelSpecs;

    property string modelID: "";

    signal post(var message);

    color: "#cccccc";
    border.color: "black";
    border.width: 2;

    width: 200;
    height: 200;

    Component.onCompleted: {
        heightChanged(height);
    }

    Grid {
        id: grid;

        columns: 3;

        anchors {
            top: modelSpecs.top;
            left: modelSpecs.left;
            margins: 5;
        }

        Text {
            text: qsTr("huRange");
        }

        Control.RangeSlider {
            id: huRangeSlider;
            width: 200;

            onValueChanged: if (modelSpecs.modelID !== "") {
                                modelSpecs.post({
                                                    "header" : {
                                                        "sender" : "modelSpecs",
                                                        "reciever" : modelSpecs.modelID
                                                    },
                                                    "data" : {
                                                        "action" : "setHuRange",
                                                        "params" : {
                                                            "range" : value
                                                        }
                                                    }
                                                });
                            }
        }

        Text {
            text: huRangeSlider.value.x.toFixed(2) + " " + huRangeSlider.value.y.toFixed(2);
        }

        Text {
            text: qsTr("windowCenter");
        }

        Control.Slider {
            id: windowCenterSlider;
            width: 200;
            stepSize: 10.0;

            onValueChanged: if (modelSpecs.modelID !== "") {
                                modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : modelSpecs.modelID
                                                },
                                                "data" : {
                                                    "action" : "setWindowCenter",
                                                    "params" : {
                                                        "windowCenter" : value
                                                    }
                                                }
                                            });
                            }
        }

        Text {
            text: windowCenterSlider.value;
        }

        Text {
            text: qsTr("windowWidth");
        }

        Control.Slider {
            id: windowWidthSlider;
            width: 200;
            stepSize: 10.0;

            onValueChanged: if (modelSpecs.modelID !== "") {
                                modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : modelSpecs.modelID
                                                },
                                                "data" : {
                                                    "action" : "setWindowWidth",
                                                    "params" : {
                                                        "windowWidth" : value
                                                    }
                                                }
                                            });
                            }
        }

        Text {
            text: windowWidthSlider.value;
        }
    }

    Grid {
        columns: 2;
        rows: 3;

        anchors {
            top: grid.bottom;
            left: modelSpecs.left;
            margins: 15;
        }

        Text {
            text: qsTr("xRange");
        }

        Control.RangeSlider {
            id: xRange;

            value: Qt.vector2d(-1.0, 1.0);
            valueRange: Qt.vector2d(-1.0, 1.0);

            onValueChanged: if (modelSpecs.modelID !== "") {
                                modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : modelSpecs.modelID
                                                },
                                                "data" : {
                                                    "action" : "setRange",
                                                    "params" : {
                                                        "range" : value,
                                                        "axis" : "x"
                                                    }
                                                }
                                            });
                            }
        }

        Text {
            text: qsTr("yRange");
        }

        Control.RangeSlider {
            id: yRange;

            value: Qt.vector2d(-1.0, 1.0);
            valueRange: Qt.vector2d(-1.0, 1.0);

            onValueChanged: if (modelSpecs.modelID !== "") {
                                modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : modelSpecs.modelID
                                                },
                                                "data" : {
                                                    "action" : "setRange",
                                                    "params" : {
                                                        "range" : value,
                                                        "axis" : "y"
                                                    }
                                                }
                                            });
                            }
        }

        Text {
            text: qsTr("zRange");
        }

        Control.RangeSlider {
            id: zRange;

            value: Qt.vector2d(-1.0, 1.0);
            valueRange: Qt.vector2d(-1.0, 1.0);

            onValueChanged: if (modelSpecs.modelID !== "") {
                                modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : modelSpecs.modelID
                                                },
                                                "data" : {
                                                    "action" : "setRange",
                                                    "params" : {
                                                        "range" : value,
                                                        "axis" : "z"
                                                    }
                                                }
                                            });
                            }
        }
    }

    function recieve(message) {
        switch (message.data.action) {
        case "changeModelID" :
            modelSpecs.modelID = message.header.sender;
            break;
        case "setSpecs" :
            var windowCenter = message.data["params"]["windowCenter"];

            windowCenterSlider.minimumValue = 0;
            windowCenterSlider.maximumValue = windowCenter * 3;
            windowCenterSlider.value = windowCenter;

            var windowWidth = message.data["params"]["windowWidth"];

            windowWidthSlider.minimumValue = 0;
            windowWidthSlider.maximumValue = windowWidth * 3;
            windowWidthSlider.value = windowWidth;

            var huRange = message.data["params"]["huRange"];
            var huRangePossible = message.data["params"]["huRangePossible"];

            huRangePossible.y = Math.min(huRangePossible.y, huRange.y * 5);

            huRangeSlider.valueRange = huRangePossible;
            huRangeSlider.value = huRange;
            break;
        }
    }
}
