import QtQuick 2.3;

import "qrc:/qml/Control" as Control;

Rectangle {
    id: modelSpecs;

    property vector2d huRangeMax: Qt.vector2d(0, 65535);
    property vector2d huRangeModel: Qt.vector2d(0, 65535);

    property int windowCenterModel: 400;
    property int windowWidthModel: 1000;

    signal post(variant message);

    color: "#cccccc";
    border.color: "black";
    border.width: 2;

    width: 200;
    height: 200;

    Grid {
        id: grid;

        columns: 3;
        rows: 12;

        anchors {
            top: modelSpecs.top;
            left: modelSpecs.left;
            margins: 5;
        }

        Text {
            text: qsTr("minHU");
        }

        Control.Slider {
            id: minHUSlider;
            width: 200;
            minimumValue: modelSpecs.huRangeMax.x;
            maximumValue: modelSpecs.huRangeMax.y;
            value: modelSpecs.huRangeModel.x;
            stepSize: 100.0;

            onValueChanged: modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : "currentModel"
                                                },
                                                "data" : {
                                                    "action" : "setMinHU",
                                                    "params" : {
                                                        "range" : value
                                                    }
                                                }
                                            });
        }

        Text {
            text: minHUSlider.value;
        }

        Text {
            text: qsTr("maxHU");
        }

        Control.Slider {
            id: maxHUSlider;
            width: 200;
            minimumValue: modelSpecs.huRangeMax.x;
            maximumValue: modelSpecs.huRangeMax.y;
            value: modelSpecs.huRangeModel.y;
            stepSize: 1.0;

            onValueChanged: modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : "currentModel"
                                                },
                                                "data" : {
                                                    "action" : "setMaxHU",
                                                    "params" : {
                                                        "range" : value
                                                    }
                                                }
                                            });
        }

        Text {
            text: maxHUSlider.value;
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

            onValueChanged: modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : "currentModel"
                                                },
                                                "data" : {
                                                    "action" : "setWindowCenter",
                                                    "params" : {
                                                        "range" : value
                                                    }
                                                }
                                            });
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

            onValueChanged: modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : "currentModel"
                                                },
                                                "data" : {
                                                    "action" : "setWindowWidth",
                                                    "params" : {
                                                        "range" : value
                                                    }
                                                }
                                            });
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

            onValueChanged: modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : "currentModel"
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

        Text {
            text: qsTr("yRange");
        }

        Control.RangeSlider {
            id: yRange;

            value: Qt.vector2d(-1.0, 1.0);
            valueRange: Qt.vector2d(-1.0, 1.0);

            onValueChanged: modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : "currentModel"
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

        Text {
            text: qsTr("zRange");
        }

        Control.RangeSlider {
            id: zRange;

            value: Qt.vector2d(-1.0, 1.0);
            valueRange: Qt.vector2d(-1.0, 1.0);

            onValueChanged: modelSpecs.post({
                                                "header" : {
                                                    "sender" : "modelSpecs",
                                                    "reciever" : "currentModel"
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
