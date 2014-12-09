import QtQuick 2.3;

import "qrc:/qml/Control" as Control;

Rectangle {
    id: modelSpecs;

    property vector2d huRangeMax: Qt.vector2d(0, 65535);
    property vector2d huRangeModel: Qt.vector2d(0, 65535);

    property int windowCenterModel: 400;
    property int windowWidthModel: 1000;

    property string modelID: "";

    signal post(variant message);

    color: "#cccccc";
    border.color: "black";
    border.width: 2;

    width: 200;
    height: 200;

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

            value: Qt.vector2d(-1.0, 1.0);
            valueRange: Qt.vector2d(-1.0, 1.0);

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
            text: huRangeSlider.value.x + " " + huRangeSlider.value.y;
        }

        Text {
            text: qsTr("windowCenter");
        }

        Control.Slider {
            id: windowCenterSlider;
            width: 200;
            minimumValue: 0;
            maximumValue: 10000;
            value: modelSpecs.windowCenterModel;
            stepSize: 100.0;

            onValueChanged: if (modelSpecs.modelID !== "") {
                                modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : modelSpecs.modelID
                                                },
                                                "data" : {
                                                    "action" : "setWindowCenter",
                                                    "params" : {
                                                        "range" : value
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
            minimumValue: 0;
            maximumValue: 10000;
            value: modelSpecs.windowWidthModel;
            stepSize: 100.0;

            onValueChanged: if (modelSpecs.modelID !== "") {
                                modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : modelSpecs.modelID
                                                },
                                                "data" : {
                                                    "action" : "setWindowWidth",
                                                    "params" : {
                                                        "range" : value
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
        }
    }
}
